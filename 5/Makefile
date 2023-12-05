.PHONY: build run

build:
	@echo "Building..."
	g++ -o solution solution.cpp

run:
	@if [ -z "$(file)" ]; then \
		echo "Error: 'file' argument missing. Usage: make run file=somefile.txt"; \
	else \
		./solution $(file); \
	fi
