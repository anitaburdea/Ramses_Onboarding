# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

CC = gcc                        # compiler to use
PACKAGES := glib-2.0

LINKERFLAG = -lm

TARGET_EXEC := training_application
SRCS_DIRS := ./src
BUILD_DIR := ./build

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INCLUDES += -I./include `pkg-config --cflags $(PACKAGES)`

CFLAGS = -g -Wall -Wpedantic -Wno-padded -O $(shell pkg-config --cflags glib-2.0)

LFLAGS = $(shell pkg-config --libs glib-2.0)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(CFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDES) -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
