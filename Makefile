CUSTOM_COMMANDS = all clean
BOARDS = ldu pcu

.PHONY: $(CUSTOM_COMMANDS) $(BOARDS) 

all: ldu pcu

BIN_DIR = Bin
BUILD_DIR = ../../$(BIN_DIR)

EXTRA_LDFLAGS = -Wl,--no-warn-rwx-segments -u _printf_float -u _scanf_float

ldu:
	@mkdir -p $(BIN_DIR)
	@make -C ldu/ BUILD_DIR=$(BUILD_DIR) EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)"
pcu:
	mkdir -p $(BIN_DIR)
	make -C pcu/ BUILD_DIR=$(BUILD_DIR) EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)"

clean:
	rm -rf $(BIN_DIR)

