#include <stdio.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

void row_matrix_product(int row[4], int matrix[4][4], int res[4]) {
	int i, j;
	for(i = 0; i < 4; i++) {
		res[i] = 0;
		for(j = 0; j < 4; j++)
			res[i] = res[i] + row[j] * matrix[j][i];
	}
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if(my_rank == 0) {
		int mat1[4][4] = {{2,3,1,5}, {7,5,1,0}, {3,6,1,8}, {1,5,6,3}}, mat2[4][4] = {{2,3,1,5}, {7,5,1,0}, {3,6,1,8}, {1,5,6,3}};
		int i;
		for(i = 1; i < world_size; i++) {
			MPI_Send(&mat1[i - 1][0], 4, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&mat2[0][0], 16, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		int mat3[4][4];
		int sender, j;
		int temp[4];
		MPI_Status status;
		for(i = 0; i < 4; i++) {
			MPI_Recv(&temp[0], 4, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			sender = status.MPI_SOURCE;
			for(j = 0; j < 4; j++)
				mat3[sender - 1][j] = temp[j];
		}
		for(i = 0; i < 4; i++) {
			for (j = 0; j <4; j++)
				printf("%d ", mat3[i][j]);
			printf("\n");
		}
	} else {
		int row[4];
		int matrix[4][4];
		MPI_Recv(&row[0], 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&matrix[0][0], 16, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int res[4];
		row_matrix_product(row, matrix, res);
		MPI_Send(&res[0], 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}