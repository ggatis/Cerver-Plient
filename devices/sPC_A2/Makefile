#Makefile for compiling the client code
SHELL    = /bin/bash

#compiler
CC = gcc

#flags and options
CFLAGS   = -Wall
LFLAGS   = -L/usr/lib/x86_64-linux-gnu/

MAIN     = Cerver
INCLUDES = -Iinc -I../../common/inc
LIBS     = -lz

MSOURCE      = src/$(MAIN).c
SOURCES      = $(MSOURCE) ../../common/src/*.c
OUTPUTS      = bin
TARGET       = $(OUTPUTS)/$(MAIN).exe
DEPENDENCIES = $(SOURCES) inc/$(MAIN).h ../../common/inc/messages_A.h

#default target
all: $(TARGET)

#compile
$(TARGET): $(DEPENDENCIES)
	@if [ -e $(MSOURCE) ]; then \
		echo Building "$(TARGET)"; \
		mkdir $(OUTPUTS); \
		$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -o $(TARGET) $(SOURCES) $(LIBS); \
	fi; \

#clean up generated files
clean:
	rm -r $(OUTPUTS) -d -f
