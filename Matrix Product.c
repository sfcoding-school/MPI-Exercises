#include <stdio.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

int row_col_prod(int row[4], int col[4]) {
	int i, res = 0;
	for(i = 0; i < 4; i++)
		res = res + row[i] * col[i];
	return res;
}
	
void get_column(int mat[4][4], int col[4], int n) {
	int i;
	for(i = 0; i <4; i++)
		col[i] = mat[i][n];
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int mat1[4][4] = {{2,3,1,5}, {7,5,1,0}, {3,6,1,8}, {1,5,6,3}}, mat2[4][4] = {{2,3,1,5}, {7,5,1,0}, {3,6,1,8}, {1,5,6,3}};
	if(my_rank == 0) {
		int mat3[4][4];
		int col[4];
		get_column(mat2, col, 0);
		mat3[0][0] = row_col_prod(mat1[0], col);
		int i, j, res, sender;
		MPI_Status status;
		for(i = 1; i < 16; i++) {
			MPI_Recv(&res, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			sender = status.MPI_SOURCE;
			int r = sender / 4;
			int c = sender % 4;
			mat3[r][c] = res;
		}
		for(i = 0; i < 4; i++) {
			for (j = 0; j <4; j++)
				printf("%d ", mat3[i][j]);
			printf("\n");
		}
	} else {
		int r = my_rank / 4;
		int c = my_rank % 4;
		int col[4];
		get_column(mat2, col, c);
		int res = row_col_prod(mat1[r], col);
		MPI_Send(&res, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}