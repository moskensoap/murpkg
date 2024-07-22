# Makefile for testing the Package and PackageList structures

# Compiler and flags
CC = gcc
CFLAGS = -Wall -I../include

# Source files
SRC = struct.c ../src/autoremove.c ../src/clean.c ../src/info.c ../src/list.c ../src/remove.c ../src/repo.c ../src/search.c ../src/show.c ../src/update.c ../src/utils.c ../src/utils_path.c

# Output executable
TARGET = struct.exe

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up build files
clean:
	rm -f $(TARGET)