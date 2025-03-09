#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

static struct gpiod_chip *_chip;
static struct gpiod_line *_line;

int main() {
	_chip = gpiod_chip_open("/dev/gpiochip1");	/* GPIO expansion */
	if (!_chip) {
		fprintf(stderr, "error: gpiod_chip_open\n");
		return -1;
	}
	_line = gpiod_chip_get_line(_chip, 5);	/* CAM_GPIO */
	if (!_line) {
		fprintf(stderr, "error: gpiod_chip_get_line\n");
		gpiod_chip_close(_chip);
		return -1;
	}

	int req = gpiod_line_request_input(_line, "extgpio_test");
	if (req) {
		fprintf(stderr, "error: gpiod_line_request_input\n");
		gpiod_chip_close(_chip);
		return -1;
	}

    int fd = gpiod_chip_get_fd(_chip);

    for(int i = 0; i < 10; i++)
    {
        int val = gpiod_line_get_value(_line);
        printf("Line value: %d, fd: %d\n", val, fd);
        sleep(1);
    }

    gpiod_chip_close(_chip);

    printf("Finished setting GPIO");
	return 0;
}
