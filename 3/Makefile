.PHONY: build run

build:
	@echo "Building..."
	g++ -o gear_ratios_solver gear_ratios_solver.cpp

run:
	@if [ -z "$(file)" ]; then \
		echo "Error: 'file' argument missing. Usage: make run file=somefile.txt"; \
	else \
		./gear_ratios_solver $(file); \
	fi
