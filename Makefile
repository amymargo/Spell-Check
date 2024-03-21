# Compiler to use
CC = gcc

# Name of the executable to be created
TARGET = spchk

# Source files used to build the program
SOURCES = spchk.c

# Default target for building the program
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET)

# Target to clean the build
clean:
	rm -f $(TARGET)

.PHONY: all run clean
