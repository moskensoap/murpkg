# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Iinclude -O3 -s

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Target executable
TARGET = murpkg

# Default target
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile the source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Install target to install the executable to /usr/bin
install: $(TARGET)
	install -Dm 0755 $(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET)
	install -Dm 0755 printsrcinfo $(DESTDIR)/usr/local/bin/printsrcinfo

# Clean target to remove generated files
clean:
	rm -rf $(BUILD_DIR)/* $(TARGET)

.PHONY: all clean
