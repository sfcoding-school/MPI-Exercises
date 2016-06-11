#include <stdio.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int dims[2] = {4, 4};
	int periods[2] = {1, 1};
	MPI_Comm cart;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart);
	int dest, source, recv;
	if(my_rank == 0)
		printf("Vertical\n");
	MPI_Cart_shift(cart, 0, 1, &source, &dest);
	MPI_Sendrecv(&my_rank, 1, MPI_INT, dest, 0, &recv, 1, MPI_INT, source, 0, cart, MPI_STATUS_IGNORE);
	printf("Sono %d ed ho ricevuto %d\n", my_rank, recv);
	MPI_Barrier(cart);
	if(my_rank == 0)
		printf("Horizontal\n");
	MPI_Cart_shift(cart, 1, 1, &source, &dest);
	MPI_Sendrecv(&my_rank, 1, MPI_INT, dest, 0, &recv, 1, MPI_INT, source, 0, cart, MPI_STATUS_IGNORE);
	printf("Sono %d ed ho ricevuto %d\n", my_rank, recv);
	MPI_Finalize();
}