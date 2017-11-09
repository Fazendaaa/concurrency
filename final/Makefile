all:
	@mpicc main.c GJ.c matrix.c -o exe -g -fopenmp -lm -Wall 
run:
	@mpirun ./exe 5
