#include <stdio.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>

#define N 100

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	double s_time = MPI_Wtime();
	int world_size, my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	double u[N + 2][N + 2], unew[N + 2][N + 2];
	int i, j;
	if(my_rank == 0) {
		for(i = 0; i < N + 2; i++) {
			for(j = 0; j < N + 1; j++) {
				u[i][j] = 0.0;
				unew[i][j] = 0.0;
			}
			u[i][N + 1] = 10.0;
			unew[i][N + 1] = 10.0;
		}
		double error = 1.0, v, temp[N + 2];
		int iter = 0, stop = -1, source, tag;
		MPI_Status status;
		while(error > 1.0e-6 && iter < 1000) {
			for(i = 1; i < world_size; i++)
				MPI_Send(&iter, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for(i = 1; i < world_size; i++) {
				MPI_Send(&u[0][0], (N + 2) * (N + 2), MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
				MPI_Send(&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			for(i = world_size; i < N + world_size; i++) {
				MPI_Recv(&temp[0], (N + 2), MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				source = status.MPI_SOURCE;
				tag = status.MPI_TAG;
				for(j = 1; j < N + 1; j++)
					unew[tag][j] = temp[j];
				if(i < N + 1)
					MPI_Send(&i, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
				else
					MPI_Send(&stop, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
			}
			error = 0.0;
			for(i = 1; i < world_size; i++)
				MPI_Send(&unew[i][0], (N + 2), MPI_DOUBLE, i, i, MPI_COMM_WORLD);
			for(i = world_size; i < N + world_size; i++) {
				MPI_Recv(&v, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				error += v;
				source = status.MPI_SOURCE;
				if(i < N + 1)
					MPI_Send(&unew[i][0], (N + 2), MPI_DOUBLE, source, i, MPI_COMM_WORLD);
				else
					MPI_Send(&u[0][0], (N + 2), MPI_DOUBLE, source, -1, MPI_COMM_WORLD);
			}
			for(i = 1; i < N + 1; i++)
				for(j = 1; j < N + 1; j++)
					u[i][j] = unew[i][j];
			iter++;
			if(iter % 100 == 0)
				printf("Error at iteration %d: %f\n", iter, error);
		}
		for(i = 1; i < world_size; i++)
			MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	} else {
		int tag, j;
		double new[N + 2], matrix[N + 2][N + 2];
		double v;
		MPI_Status status;
		while(1) {
			MPI_Recv(&tag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(tag == -1)
				break;
			MPI_Recv(&matrix[0][0], (N + 2) * (N + 2), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			while(1) {
				MPI_Recv(&tag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if(tag == -1)
					break;
				new[0] = 0.0;
				new[N + 1] = 0.0;
				for(j = 1; j < N + 1; j++)
					new[j] = 0.25 * (matrix[tag + 1][j] + matrix[tag - 1][j] + matrix[tag][j + 1] + matrix[tag][j - 1]);
				MPI_Send(&new, (N + 2), MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
			}
			while(1) {
				MPI_Recv(&new, (N + 2), MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				tag = status.MPI_TAG;
				v = 0.0;
				if(tag == -1)
					break;
				for(j = 1; j < N + 1; j++)
					v += fabs(new[j] - matrix[tag][j]);
				MPI_Send(&v, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			}
		}
	}
	printf("Time for process %d: %f\n", my_rank, MPI_Wtime() - s_time);
	MPI_Finalize();
}