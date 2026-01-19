#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include <time.h>

//  συνάρτηση για μέτρηση χρόνου
double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

// Σειριακός Αλγόριθμος (Ο(n^2))
void serial_mult(int n, int *A, int *B, long long *C) {
    // Αρχικοποίηση
    for (int i = 0; i <= 2 * n; i++) C[i] = 0;

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            C[i + j] += (long long)A[i] * B[j];
        }
    }
}

// Παράλληλος Αλγόριθμος OpenMP
void parallel_mult_omp(int n, int num_threads, int *A, int *B, long long *C) {
    // Ρύθμιση αριθμού νημάτων
    omp_set_num_threads(num_threads);


    #pragma omp parallel for schedule(dynamic)
    for (int k = 0; k <= 2 * n; k++) {
        long long sum = 0;
        
     
        int start_i = (k - n > 0) ? (k - n) : 0;
        int end_i = (k < n) ? k : n;

        for (int i = start_i; i <= end_i; i++) {
            sum += (long long)A[i] * B[k - i];
        }
        C[k] = sum;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <degree n> <num_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    struct timespec start, end;
    double t_init, t_serial, t_parallel;


    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int *A = (int *)malloc((n + 1) * sizeof(int));
    int *B = (int *)malloc((n + 1) * sizeof(int));
    long long *C_serial = (long long *)malloc((2 * n + 1) * sizeof(long long));
    long long *C_parallel = (long long *)malloc((2 * n + 1) * sizeof(long long));

    if (!A || !B || !C_serial || !C_parallel) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }

    // Τυχαίοι ακέραιοι 
    srand(time(NULL));
    for (int i = 0; i <= n; i++) {
        do { A[i] = rand() % 100 - 50; } while (A[i] == 0);
        do { B[i] = rand() % 100 - 50; } while (B[i] == 0);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_init = get_time_diff(start, end);

    // --- Σειριακή εκτέλεση ---
    clock_gettime(CLOCK_MONOTONIC, &start);
    serial_mult(n, A, B, C_serial);
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_serial = get_time_diff(start, end);

    // --- Παράλληλη εκτέλεση (OpenMP) ---
    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_mult_omp(n, num_threads, A, B, C_parallel);
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_parallel = get_time_diff(start, end);

    // --- Έλεγχος Ορθότητας ---
    int correct = 1;
    for (int i = 0; i <= 2 * n; i++) {
        if (C_serial[i] != C_parallel[i]) {
            correct = 0;
            printf("Mismatch at index %d: Serial=%lld, Parallel=%lld\n", i, C_serial[i], C_parallel[i]);
            break;
        }
    }

   
    printf("Degree: %d, Threads: %d\n", n, num_threads);
    printf("Init Time: %.6f sec\n", t_init);
    printf("Serial Time: %.6f sec\n", t_serial);
    printf("Parallel Time: %.6f sec\n", t_parallel);
    printf("Verification: %s\n", correct ? "SUCCESS" : "FAILURE");

    free(A); free(B); free(C_serial); free(C_parallel);

    return 0;
}
