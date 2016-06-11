#include <stdio.h>
#include <stdlib.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	srand(my_rank);
	int num = rand() % 100;
	printf("Process %d took number %d\n", my_rank, num);
	int i = 2, buff;
	while(i <= world_size) {
		if(my_rank % i == 0) {
			MPI_Recv(&buff, 1, MPI_INT, (my_rank + (i / 2)), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			num += buff;
		} else {
			MPI_Send(&num, 1, MPI_INT, (my_rank - (i / 2)), 0, MPI_COMM_WORLD);
			break;
		}
		i *= 2;
	}
	if(my_rank == 0)
		printf("Total sum: %d\n", num);
	MPI_Finalize();
}