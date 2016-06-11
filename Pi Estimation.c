#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>
#define SEED 35791246

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	double s_time = MPI_Wtime(), pi;
	int world_size, my_rank, count = 0, tot, iter = 100000;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	srand(SEED);
	if(my_rank == 0) {
		int i, nwork, rest;
		nwork = iter / (world_size - 1);
		rest = iter - (nwork * (world_size - 1)) + nwork;
		for(i = 1; i < world_size - 1; i++)
			MPI_Send(&nwork, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		MPI_Send(&rest, 1, MPI_INT, (world_size - 1), 0, MPI_COMM_WORLD);
	} else {
		int msg, i;
		double x, y, z;
		MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(i = 0; i < msg; i++) {
			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;
			z = x * x + y * y;
			if(z <= 1)
				count++;
		}
	}
	MPI_Reduce(&count, &tot, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if(my_rank == 0) {
		pi = (double)tot / iter * 4;
		printf("Estimated PI: %f\n", pi);
	}
	printf("Time for process %d: %f\n", my_rank, MPI_Wtime() - s_time);
	MPI_Finalize();
}