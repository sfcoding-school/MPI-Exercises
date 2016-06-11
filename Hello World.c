#include <stdio.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int world_size, my_rank, msg;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if(my_rank != 0)
		MPI_Recv(&msg, 1, MPI_INT, (my_rank - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	printf("Hello from processor %d out of %d\n", my_rank, world_size);
	if(my_rank != world_size - 1)
		MPI_Send(&msg, 1, MPI_INT, (my_rank + 1), 0, MPI_COMM_WORLD);
	MPI_Finalize();
}