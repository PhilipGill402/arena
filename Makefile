# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# Sources and objects
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,src/%.o,$(SRC))

# Output binary
TARGET = allocate 

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile .c to .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f src/*.o $(TARGET)

