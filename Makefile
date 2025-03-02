CUSTOM_COMMANDS = all clean
BOARDS = ldu pcu raspi

.PHONY: $(CUSTOM_COMMANDS) $(BOARDS) 

all: ldu pcu raspi matrix

board ?=
BIN_DIR = Bin
BUILD_DIR = ../../$(BIN_DIR)

BIN_FILE = $(BIN_DIR)/$(board)/$(board).elf

EXTRA_LDFLAGS = -Wl,--no-warn-rwx-segments -u _printf_float -u _scanf_float

ldu:
	@mkdir -p $(BIN_DIR)
	@make -C ldu/ BUILD_DIR=$(BUILD_DIR) EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)" --no-print-directory
pcu:
	@mkdir -p $(BIN_DIR)
	@make -C pcu/ BUILD_DIR=$(BUILD_DIR) EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)" --no-print-directory

RGB_LIB_DISTRIBUTION:=$(abspath rpi-rgb-led-matrix)
RGB_INCDIR:=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR:=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME:=rgbmatrix
RGB_LIBRARY:=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
RGB_LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

matrix:
	@make -C $(RGB_LIBDIR)

raspi:
	@make -C raspi/ BUILD_DIR="$(BUILD_DIR)/raspi" RGB_INCDIR="$(RGB_INCDIR)" RGB_LIBRARY="$(RGB_LIBRARY)" RGB_LDFLAGS="$(RGB_LDFLAGS)" --no-print-directory

flash:
	STM32_Programmer_CLI -c port=SWD -w $(BIN_FILE) 0x08000000 -v -hardRst

clean:
	rm -rf $(BIN_DIR)