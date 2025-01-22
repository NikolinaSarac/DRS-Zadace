#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <omp.h>

#define N 1000000

void sequential_sum(int* A, int size) {
    for (int i = 1; i < size; i++) {
        A[i] = A[i] + A[i-1];
    }
}

void parallel_sum(int* A, int size) {
    #pragma omp parallel for
    for (int i = 1; i < size; i++) {
        A[i] = A[i] + A[i-1];
    }
}

int main() {

    omp_set_num_threads(5);

    int *A1, *A2, *A3, *A4;
    A1 = (int*)malloc(N * sizeof(int));
    A2 = (int*)malloc(N * sizeof(int));
    A3 = (int*)malloc(N * sizeof(int));
    A4 = (int*)malloc(N * sizeof(int));

    if (A1 == NULL || A2 == NULL || A3 == NULL || A4 == NULL) {
        printf("Greška pri alokaciji memorije.\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        A1[i] = rand() % 100;
        A2[i] = rand() % 100;
        A3[i] = rand() % 100;
        A4[i] = rand() % 100;
    }

    printf("Broj niti postavljen na: %d\n ", omp_get_max_threads());

    double start_time = omp_get_wtime();
    sequential_sum(A1, N);
    sequential_sum(A2, N);
    sequential_sum(A3, N);
    sequential_sum(A4, N);
    double end_time = omp_get_wtime();
    printf("Sekvencijalno vrijeme: %f sekundi\n", end_time - start_time);

    start_time = omp_get_wtime();
    #pragma omp parallel
    {
        printf("Izvršava nit %d od ukupno %d\n", omp_get_thread_num(),omp_get_num_threads());
    }

    parallel_sum(A1, N);
    parallel_sum(A2, N);
    parallel_sum(A3, N);
    parallel_sum(A4, N);
    end_time = omp_get_wtime();
    printf("Paralelno vrijeme: %f sekundi\n", end_time - start_time);

    free(A1);
    free(A2);
    free(A3);
    free(A4);

    return 0;
}