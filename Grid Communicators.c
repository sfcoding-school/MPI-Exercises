#include <stdio.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>
#define NROW 3
#define NCOL 4

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Group world_group;
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);
	int irow = my_rank % NROW, row_list[NCOL], i;
	row_list[0] = 0;
	for(i = 1; i < NCOL; i++)
		row_list[i] = row_list[i - 1] + NROW;
	MPI_Group tmp_group;
	MPI_Comm row_comm, tmp_comm;
	int j;
	for(i = 0; i < NROW; i++) {
		MPI_Group_incl(world_group, NCOL, row_list, &tmp_group);
		MPI_Comm_create(MPI_COMM_WORLD, tmp_group, &tmp_comm);
		if(irow == i)
			MPI_Comm_dup(tmp_comm, &row_comm);
		for(j = 0; j < NCOL; j++)
			row_list[j] += 1;
	}
	int icol = my_rank / NROW, col_list[NROW];
	for(i = 0; i < NROW; i++)
		col_list[i] = i;
	MPI_Comm col_comm;
	for(i = 0; i < NCOL; i++) {
		MPI_Group_incl(world_group, NROW, col_list, &tmp_group);
		MPI_Comm_create(MPI_COMM_WORLD, tmp_group, &tmp_comm);
		if(icol == i)
			MPI_Comm_dup(tmp_comm, &col_comm);
		for(j = 0; j < NROW; j++)
			col_list[j] += NROW;
	}
	int row_rank, col_rank;
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_rank(col_comm, &col_rank);
	printf("I was process %d in COMM_WORLD, but now I am process %d in row_comm %d and process %d in col_comm %d\n", my_rank, row_rank, irow, col_rank, icol);
	MPI_Finalize();
}