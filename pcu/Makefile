BUILD_TARGET = pcu
BOARD_NAME = pcu

COMMON_LIB_SRC :=  
CUBE_MAKEFILE_PATH = Cube-files

BUILD_DIR ?= ../Bin
EXTRA_LDFLAGS ?=

SRC_DIR := $(abspath Src)
SRC := $(wildcard $(SRC_DIR)/*.c)

INC_DIR := -I$(abspath Inc)

COLORS_ENABLED ?= 1
ifeq ($(COLORS_ENABLED), 1)
GREEN_COLOR = "\\033[92m"
BLUE_COLOR = "\\033[34m"
RED_COLOR = "\\033[91m"
NO_COLOR = "\\033[0m"
endif

all:
	@echo -e "$(BLUE_COLOR)Building: $(RED_COLOR)$(BUILD_TARGET)$(NO_COLOR)"
	@make -C $(CUBE_MAKEFILE_PATH) BUILD_DIR="$(BUILD_DIR)/$(BOARD_NAME)" EXTRA_C_SRC=$(SRC) EXTRA_C_INC=$(INC_DIR) TARGET=$(BUILD_TARGET) EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)" --no-print-directory
	@echo -e "$(GREEN_COLOR)Completed$(NO_COLOR)"