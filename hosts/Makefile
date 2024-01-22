SHELL=/bin/bash

#default target
all:
	@for f in */; do \
		if [ -e "$$f/Makefile" ]; then \
			echo Building "$$f"; \
			make -C "$$f"; \
		fi; \
	done;

#clean up generated files
clean:
	@for f in */; do \
		if [ -e "$$f/Makefile" ]; then \
			echo Cleaning "$$f"; \
			make -C "$$f" clean; \
		fi; \
	done;
