#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "led.h"

// Raspberry Pi 2 or 1 ? Since this is a simple example, we don't
// bother auto-detecting but have it a compile-time option.
#ifndef PI_VERSION
#define PI_VERSION 3
#endif

#define BCM2708_PI1_PERI_BASE 0x20000000
#define BCM2709_PI2_PERI_BASE 0x3F000000
#define BCM2711_PI4_PERI_BASE 0xFE000000
#define BCM2712_PI5_PERI_BASE 0x1f000d0000

// --- General, Pi-specific setup.
#if PI_VERSION == 1
#define PERI_BASE BCM2708_PI1_PERI_BASE
#elif PI_VERSION == 2 || PI_VERSION == 3
#define PERI_BASE BCM2709_PI2_PERI_BASE
#elif PI_VERSION == 4
#define PERI_BASE BCM2711_PI4_PERI_BASE
#else
#define PERI_BASE BCM2712_PI5_PERI_BASE
#endif

#define PAGE_SIZE 4096

// ---- GPIO specific defines
#define GPIO_REGISTER_BASE 0x200000
#define GPIO_SET_OFFSET 0x1C
#define GPIO_CLR_OFFSET 0x28
#define PHYSICAL_GPIO_BUS (0x7E000000 + GPIO_REGISTER_BASE)

// Return a pointer to a periphery subsystem register.
static void *mmap_bcm_register(off_t register_offset) {
    const off_t base = PERI_BASE;

    int mem_fd;
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("can't open /dev/mem: ");
        fprintf(stderr, "You need to run this as root!\n");
        return NULL;
    }

    uint32_t *result =
        (uint32_t *)mmap(NULL,  // Any adddress in our space will do
                         PAGE_SIZE,
                         PROT_READ | PROT_WRITE,  // Enable r/w on GPIO registers.
                         MAP_SHARED,
                         mem_fd,                 // File to map
                         base + register_offset  // Offset to bcm register
        );
    close(mem_fd);

    if (result == MAP_FAILED) {
        fprintf(stderr, "mmap error %p\n", result);
        return NULL;
    }
    return result;
}

void initialize_gpio_for_output(volatile uint32_t *gpio_registerset, int bit) {
    *(gpio_registerset + (bit / 10)) &= ~(7 << ((bit % 10) * 3));  // prepare: set as input
    *(gpio_registerset + (bit / 10)) |= (1 << ((bit % 10) * 3));   // set as output.
}

static inline void my_sleep(uint16_t nops) {
    for (uint16_t i = 0; i < nops; i++)
        __asm volatile("nop\n");
}

void gpio_test() {
    // Prepare GPIO
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *set_reg = gpio_port + (GPIO_SET_OFFSET / sizeof(uint32_t));
    volatile uint32_t *clr_reg = gpio_port + (GPIO_CLR_OFFSET / sizeof(uint32_t));

    const unsigned CLK_PIN = 27;
    const unsigned DAT_PIN = 3;
    const unsigned LAT_PIN = 22;
    initialize_gpio_for_output(gpio_port, CLK_PIN);
    initialize_gpio_for_output(gpio_port, DAT_PIN);
    initialize_gpio_for_output(gpio_port, LAT_PIN);

    // clock out the alternating high and low
    // Do it. Endless loop, directly setting.
    printf(
        "1) CPU: Writing to GPIO directly in tight loop\n"
        "== Press Ctrl-C to exit.\n");

    uint8_t addr = 0;

    for (;;) {
        uint8_t ctrl = 0;
        uint8_t data = (ctrl << 5) | addr;
        addr = (addr + 1) % 32;
        for (uint8_t i = 0; i < 8; i++) {
            if (data & (1 << i))
                *set_reg = 1 << DAT_PIN;
            else
                *clr_reg = 1 << DAT_PIN;
            *set_reg = 1 << CLK_PIN;
            *clr_reg = 1 << CLK_PIN;
        }
        *set_reg = 1 << LAT_PIN;
        *clr_reg = 1 << LAT_PIN;
        //      printf("%d\n", addr);
        //      my_sleep(50000);
    }
}

void counter_test() {
    // Prepare GPIO
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *set_reg = gpio_port + (GPIO_SET_OFFSET / sizeof(uint32_t));
    volatile uint32_t *clr_reg = gpio_port + (GPIO_CLR_OFFSET / sizeof(uint32_t));

    const unsigned CLK_PIN = 4;
    initialize_gpio_for_output(gpio_port, CLK_PIN);
    
    while (1) {
        *set_reg = 1 << CLK_PIN;
        *clr_reg = 1 << CLK_PIN;
    }
}

int main(int argc, char **argv) {
    counter_test();
    // test_led();
    return 0;
}