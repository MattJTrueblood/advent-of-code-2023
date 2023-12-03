.PHONY: build run

build:
	@echo "Building..."
	g++ -o trebuchet_solver trebuchet_solver.cpp

run:
	@if [ -z "$(file)" ]; then \
		echo "Error: 'file' argument missing. Usage: make run file=somefile.txt"; \
	else \
		./trebuchet_solver $(file); \
	fi
