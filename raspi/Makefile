CC = gcc
C_FLAGS = -std=c11 -Wall -O3

BUILD_DIR = ../Bin/raspi
RGB_INCDIR ?=
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

INC = -I$(INC_DIR) -I$(RGB_INCDIR)

# $(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
# 	$(CC) -c $(C_FLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@ $(RGB_LDFLAGS)

$(BUILD_DIR)/voxel:
	@mkdir -p $(BUILD_DIR)
	$(CC) $(C_SOURCES) $(INC) $(C_FLAGS) -o $@  $(RGB_LDFLAGS) 