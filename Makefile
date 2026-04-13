# Variables
CC = gcc
CFLAGS = -Wall -g
OBJ_DIR = obj/Debug
TARGET = $(OBJ_DIR)/main.o

# Default target
all: $(TARGET)

# Rule to create the object file
$(TARGET): main.c
	@# Ensure the directory exists before compiling
	if not exist "obj\Debug" mkdir "obj\Debug"
	$(CC) $(CFLAGS) -c ./main.c -o $(TARGET)

# Clean rule to remove the build artifacts
clean:
	rmdir /s /q obj