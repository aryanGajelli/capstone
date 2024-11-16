CUSTOM_COMMANDS = all clean
BOARDS = ldu pcu

.PHONY: $(CUSTOM_COMMANDS) $(BOARDS) 

all: ldu pcu

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

flash:
	STM32_Programmer_CLI -c port=SWD -w $(BIN_FILE) 0x08000000 -v -hardRst

clean:
	rm -rf $(BIN_DIR)

