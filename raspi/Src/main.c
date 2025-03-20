#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "gpio.h"
#include "volumetric.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <volume_file.xyzrgb>\n", argv[0]);
        return 1;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    return volumetric_test(fptr);
}