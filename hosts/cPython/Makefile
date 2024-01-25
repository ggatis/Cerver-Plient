#Makefile for compiling the client code
SHELL    = /bin/bash

#compiler
CC = pyinstaller

MAIN         = Plient_A3
MSOURCE      = src/$(MAIN).py
SOURCES      = $(MSOURCE)
OUTPUTS      = ./bin
TARGET       = $(OUTPUTS)/$(MAIN).exe
WORKDIR      = ./build
DEPENDENCIES = $(SOURCES)

#flags and options
CFLAGS   = --clean --onefile --distpath $(OUTPUTS) --workpath $(WORKDIR) --specpath $(WORKDIR)


#default target
all: $(TARGET)

#compile
$(TARGET): $(DEPENDENCIES)
	if [ -e $(MSOURCE) ]; then \
		echo Building "$(TARGET)"; \
		mkdir $(OUTPUTS); \
		$(CC) $(SOURCES) $(CFLAGS) -n $(MAIN).exe; \
	fi; \

#clean up generated files
clean:
	rm -r $(WORKDIR) -d -f; \
	rm -r $(OUTPUTS) -d -f
