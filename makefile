# Makefile for compiling the UDP server code

#compiler
CC = gcc

#flags and options
CFLAGS = -Wall
LFLAGS = -L/usr/lib/x86_64-linux-gnu/
LIBS   = -lz

#source file
SERVER_SOURCE = Cerver.c

#output executable
SERVER_EXECUTABLE = Cerver.exe

#default target
all: $(SERVER_EXECUTABLE)

#compile
$(SERVER_EXECUTABLE): $(SERVER_SOURCE)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(SERVER_EXECUTABLE) $(SERVER_SOURCE) $(LIBS)

#clean up generated files
clean:
	rm -f $(SERVER_EXECUTABLE)
