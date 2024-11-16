BUILD_TARGET = ldu
BOARD_NAME = ldu

COMMON_DIR := $(abspath ../common)
COMMON_INC := -I$(COMMON_DIR)/Inc
COMMON_LIB_SRC :=  $(wildcard $(COMMON_DIR)/Src/*.c)
CUBE_MAKEFILE_PATH = Cube-files

BUILD_DIR ?= ../Bin
EXTRA_LDFLAGS ?=

SRC_DIR := $(abspath Src)
SRC := $(wildcard $(SRC_DIR)/*.c) $(COMMON_LIB_SRC)

INC_DIR := -I$(abspath Inc) $(COMMON_INC)

COLORS_ENABLED ?= 1
ifeq ($(COLORS_ENABLED), 1)
GREEN_COLOR = "\\033[92m"
BLUE_COLOR = "\\033[34m"
RED_COLOR = "\\033[91m"
NO_COLOR = "\\033[0m"
endif

all:
	@echo -e "$(BLUE_COLOR)Building: $(RED_COLOR)$(BUILD_TARGET)$(NO_COLOR)"
	@make -C $(CUBE_MAKEFILE_PATH) BUILD_DIR="$(BUILD_DIR)/$(BOARD_NAME)" EXTRA_C_SRC="$(SRC)" EXTRA_C_INC="$(INC_DIR)" TARGET=$(BUILD_TARGET) EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)" --no-print-directory
	@echo -e "$(GREEN_COLOR)Completed$(NO_COLOR)"