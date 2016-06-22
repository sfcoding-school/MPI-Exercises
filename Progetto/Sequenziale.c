#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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
	time_t s_time = time(NULL);
	int d, i, j, k, l, indx[N];
	double dum;
	l = m1;
	double **a, **al;
	create_matrix(&a, &al);
	for(i = 0; i < m1; i++) {
		for(j = m1 - i; j < mm; j++)
			a[i][j - l] = a[i][j];
		l--;
		for(j = mm - l - 1; j < mm; j++)
			a[i][j] = 0.0;
	}
	d = 1;
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
		for(i = k + 1; i < l; i++) {
			dum = a[i][0] / a[k][0];
			al[k][i - k - 1] = dum;
			for(j = 1; j < mm; j++)
				a[i][j - 1] = a[i][j] - dum * a[k][j];
			a[i][mm - 1] = 0.0;
		}
	}
	for(i = 0; i < N; i++) {
		free(a[i]);
		free(al[i]);
	}
	free(a);
	free(al);
	printf("Execution time: %f\n", (double) (time(NULL) - s_time));
}
#undef SWAP
#undef TINY