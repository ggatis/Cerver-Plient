# Makefile for compiling the UDP server code

# Compiler
CC = gcc

# Compiler flags
# CFLAGS = -Wall -L/usr/lib/x86_64-linux-gnu/ -lz
CFLAGS = -Wall -lz

# Source file
SERVER_SOURCE = Cerver.c

# Output executable
SERVER_EXECUTABLE = Cerver.exe

# Default target (compiles the server)
all: $(SERVER_EXECUTABLE)

# Compile the server
$(SERVER_EXECUTABLE): $(SERVER_SOURCE)
	$(CC) $(CFLAGS) -o $(SERVER_EXECUTABLE) $(SERVER_SOURCE)

# Clean up generated files
clean:
	rm -f $(SERVER_EXECUTABLE)
