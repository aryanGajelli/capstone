CC = gcc
C_FLAGS = -std=c++11 -Wall -O3

BUILD_DIR ?= ../Bin
RGB_LIBRARY ?=
RGB_LDFLAGS ?=


# Our stuff
SRC_DIR = $(abspath Src)
INC_DIR = $(abspath Inc)
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)


OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# $(info $(OBJECTS))
# # $(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
# # 	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@


all:
	$(CC) -c $(C_FLAGS) $(OBJECTS) -o $@ $(RGB_LDFLAGS)