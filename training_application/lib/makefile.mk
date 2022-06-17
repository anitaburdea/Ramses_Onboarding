# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

CC = gcc                        # compiler to use
PACKAGES := glib-2.0 gio-unix-2.0

LIB_NAME := lib_dbus_client.so
SRC_DIRS := ./source
OUT_FOLDER := ./bin
BUILD_DIR := ./build

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INCLUDES += -I./include `pkg-config --cflags $(PACKAGES)`
INCLUDES += -I./include

LDFLAGS += `pkg-config --libs $(PACKAGES)` 

$(LIB_NAME):$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(OUT_FOLDER)/$@ -shared -fPIC 

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFFLAGS) $(INCLUDES) -c $< -o $@

all:$(LIB_NAME)

.PHONY: clean
clean:rm -rf $(OUT_FOLDER)/*.o
