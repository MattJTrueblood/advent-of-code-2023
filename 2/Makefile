.PHONY: build run

build:
	@echo "Building..."
	g++ -o cube_conundrum_solver cube_conundrum_solver.cpp

run:
	@if [ -z "$(file)" ]; then \
		echo "Error: 'file' argument missing. Usage: make run file=somefile.txt"; \
	else \
		./cube_conundrum_solver $(file); \
	fi
