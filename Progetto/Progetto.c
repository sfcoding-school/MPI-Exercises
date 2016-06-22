#include <stdio.h>
#include <math.h>
#include </opt/lib/mpi/intel/14.0.2/mvapich2/1.9/include/mpi.h>
#define N 5000
#define m1 800
#define m2 400
#define mm (m1 + m2 + 1)
#define SWAP(c,b) {dum=(c);(c)=(b);(b)=dum;}
#define TINY 1.0e-20

void create_matrix(double ***a, double ***al) {
	(*a) = (double **) malloc(N * sizeof(double *));
	(*al) = (double **) malloc(N * sizeof(double *));
	int i, j;
	for(i = 0; i < N; i++) {
		(*al)[i] = (double *) malloc(m1 * sizeof(double));
		(*a)[i] = (double *) malloc(mm * sizeof(double));
	}
	for(i = 0; i < N; i++)
		for(j = 0; j < m1; j++)
			(*al)[i][j] = 0.0;
	for(i = 0; i < m1; i++) {
		for(j = 0; j < m1 - i; j++)
			(*a)[i][j] = 0.0;
		for(j = m1 - i; j < mm; j++)
			(*a)[i][j] = (double) (j + 1);
	}
	for(i = m1; i < N - m2; i++)
		for(j = 0; j < mm; j++)
			(*a)[i][j] = (double) (j + 1);
	for(i = N - m2; i < N; i++) {
		for(j = 0; j < mm - (N - i); j++)
			(*a)[i][j] = (double) (j + 1);
		for(j = mm - (N - i); j < mm; j++)
			(*a)[i][j] = 0.0;
	}
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	double s_time = MPI_Wtime();
	int world_size, my_rank, d = 1, i, j, k, l, tag;
	double dum;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Datatype rowtype;
	MPI_Status status;
	MPI_Type_contiguous(mm, MPI_DOUBLE, &rowtype);
	MPI_Type_commit(&rowtype);
	if(my_rank == 0) {
		int min = world_size - 1, sender, indx[N];
		double **a, **al;
		create_matrix(&a, &al);
		l = m1;
		if(min > m1)
			min = m1;
		MPI_Request req_list[world_size - 1];
		for(i = 1; i < min + 1; i++)
			MPI_Isend(&a[i - 1][0], 1, rowtype, i, i - 1, MPI_COMM_WORLD, &req_list[i - 1]);
		for(i = min + 1; i < world_size; i++)
			MPI_Isend(&a[0][0], 1, rowtype, i, -1, MPI_COMM_WORLD, &req_list[i - 1]);
		for(i = min; i < m1 + min; i++) {
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			sender = status.MPI_SOURCE;
			tag = status.MPI_TAG;
			MPI_Recv(&a[tag][0], 1, rowtype, sender, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(i < m1)
				MPI_Isend(&a[i][0], 1, rowtype, sender, i, MPI_COMM_WORLD, &req_list[sender]);
			else
				MPI_Isend(&a[0][0], 1, rowtype, sender, -1, MPI_COMM_WORLD, &req_list[sender]);
		}
		l = m1;
		for(k = 0; k < N; k++) {
			dum = a[k][0];
			i = k;
			if(l < N)
				l++;
			for(j = k + 1; j < l; j++) {
				if(fabs(a[j][0]) > fabs(dum)) {
					dum = a[j][0];
					i = j;
				}
			}
			indx[k] = i;
			if(dum = 0.0)
				a[k][0] = TINY;
			if(i != k) {
				d = -d;
				for(j = 0; j < mm; j++)
					SWAP(a[k][j], a[i][j]);
			}
			min = world_size - 1;
			if(min > l - k - 1)
				min = l - k - 1;
			MPI_Waitall(world_size - 1, req_list, MPI_STATUSES_IGNORE);
			for(i = 1; i < min + 1; i++) {
				dum = a[i + k][0] / a[k][0];
				al[k][i - 1] = dum;
				MPI_Isend(&a[k][0], 1, rowtype, i, 0, MPI_COMM_WORLD, &req_list[i - 1]);
				MPI_Isend(&a[i + k][0], 1, rowtype, i, i + k, MPI_COMM_WORLD, &req_list[i - 1]);
			}
			for(i = min + 1; i < world_size; i++)
				req_list[i - 1] = MPI_REQUEST_NULL;
			for(i = min + 1; i < l - k + min; i++) {
				MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				sender = status.MPI_SOURCE;
				tag = status.MPI_TAG;
				MPI_Recv(&a[tag][0], 1, rowtype, sender, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if(i < l - k) {
					dum = a[i + k][0] / a[k][0];
					al[k][i - 1] = dum;
					MPI_Isend(&a[i + k][0], 1, rowtype, sender, i + k, MPI_COMM_WORLD, &req_list[sender]);
				} else
					MPI_Isend(&a[0][0], 1, rowtype, sender, -1, MPI_COMM_WORLD, &req_list[sender]);
			}
		}
		for(i = 1; i < world_size; i++)
			MPI_Isend(&a[0][0], 1, rowtype, i, -2, MPI_COMM_WORLD, &req_list[i - 1]);
	} else {
		double temp[mm], act[mm];
		while(1) {
			MPI_Recv(&temp[0], 1, rowtype, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			tag = status.MPI_TAG;
			if(tag == -1)
				break;
			l = m1 - tag;
			for(j = m1 - tag; j < mm; j++)
				temp[j - l] = temp[j];
			l--;
			for(j = mm - l - 1; j < mm; j++)
				temp[j] = 0.0;
			MPI_Send(&temp[0], 1, rowtype, 0, tag, MPI_COMM_WORLD);
		}
		while(1) {
			MPI_Recv(&act[0], 1, rowtype, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			tag = status.MPI_TAG;
			if(tag == -2)
				break;
			while(tag != -1) {
				MPI_Recv(&temp[0], 1, rowtype, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				tag = status.MPI_TAG;
				if(tag == -1)
					break;
				dum = temp[0] / act[0];
				for(j = 1; j < mm; j++)
					temp[j - 1] = temp[j] - dum * act[j];
				temp[mm - 1] = 0.0;
				MPI_Send(&temp[0], 1, rowtype, 0, tag, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Type_free(&rowtype);
	printf("Time for process %d: %f\n", my_rank, MPI_Wtime() - s_time);
	MPI_Finalize();
}
#undef SWAP
#undef TINY