# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

CC = gcc                        # compiler to use
PACKAGES := glib-2.0 gio-unix-2.0

LINKERFLAG = -lm

TARGET_EXEC := training_application
SRCS_DIRS := ./src
BUILD_DIR := ./build

SRCS := $(shell find $(SRCS_DIRS) -name '*.c')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INCLUDES += -I./include
INCLUDES += -I./lib/include
INCLUDES += -I./include `pkg-config --cflags $(PACKAGES)`

LDFLAGS = `pkg-config --libs $(PACKAGES)` \
           -L./lib/bin -l_dbus_client

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) -rdynamic

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(INCLUDES) -c $< -o $@

all:
	${BUILD_DIR}

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
