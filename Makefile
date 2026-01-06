CC = gcc
CFLAGS = -Wall -O3
LIBS_OMP = -fopenmp

all: poly_mult_omp

poly_mult_omp: poly_mult_omp.c
	$(CC) $(CFLAGS) $(LIBS_OMP) -o poly_mult_omp poly_mult_omp.c

clean:
	rm -f poly_mult_omp