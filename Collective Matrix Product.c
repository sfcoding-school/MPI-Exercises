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
	int mat1[N][N], mat2[N][N], mat3[N][N], i;
	if(my_rank == 0) {
		int j;
		for(i = 0; i < N; i++)
			for(j = 0; j < N; j++) {
				mat1[i][j] = rand() % 20;
				mat2[i][j] = rand() % 20;
			}
	}
	MPI_Bcast(&mat2[0][0], N * N, MPI_INT, 0, MPI_COMM_WORLD);
	int iter = N / world_size, row[N], res[N];
	for(i = 0; i < iter; i++) {
		MPI_Scatter(&mat1[i * world_size][0], N, MPI_INT, &row, N, MPI_INT, 0, MPI_COMM_WORLD);
		row_matrix_product(row, mat2, res);
		MPI_Gather(&res, N, MPI_INT, &mat3[i * world_size][0], N, MPI_INT, 0, MPI_COMM_WORLD);
	}
	printf("Time for process %d: %f\n", my_rank, MPI_Wtime() - s_time);
	MPI_Finalize();
}