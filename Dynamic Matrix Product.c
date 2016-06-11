#include <stdio.h>
#include <stdlib.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

#define N 100

void row_matrix_product(int row[N], int matrix[N][N], int res[N]) {
	int i, j;
	for(i = 0; i < N; i++) {
		res[i] = 0;
		for(j = 0; j < N; j++)
			res[i] = res[i] + row[j] * matrix[j][i];
	}
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	double s_time = MPI_Wtime();
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if(my_rank == 0) {
		int mat1[N][N], mat2[N][N];
		int i, j;
		for(i = 0; i < N; i++)
			for(j = 0; j < N; j++) {
				mat1[i][j] = rand() % 20;
				mat2[i][j] = rand() % 20;
			}
		for(i = 1; i < world_size; i++) {
			MPI_Send(&mat2[0][0], N * N, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&mat1[i - 1][0], N, MPI_INT, i, i - 1, MPI_COMM_WORLD);
		}
		int mat3[N][N];
		int row, sender;
		int temp[N];
		MPI_Status status;
		for(i = world_size - 1; i < N + world_size - 1; i++) {
			MPI_Recv(&temp[0], N, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			row = status.MPI_TAG;
			sender = status.MPI_SOURCE;
			for(j = 0; j < N; j++)
				mat3[row][j] = temp[j];
			if(i < N)
				MPI_Send(&mat1[i][0], N, MPI_INT, sender, i, MPI_COMM_WORLD);
			else
				MPI_Send(&mat1[0][0], N, MPI_INT, sender, -1, MPI_COMM_WORLD);
		}
	} else {
		int row[N];
		int matrix[N][N];
		MPI_Recv(&matrix[0][0], N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int res[N];
		int tag;
		MPI_Status status;
		while(1) {
			MPI_Recv(&row[0], N, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			tag = status.MPI_TAG;
			if(tag == -1)
				break;
			row_matrix_product(row, matrix, res);
			MPI_Send(&res[0], N, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
	}
	printf("Time for process %d: %f\n", my_rank, MPI_Wtime() - s_time);
	MPI_Finalize();
}