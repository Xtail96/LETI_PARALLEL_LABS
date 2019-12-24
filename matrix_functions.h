
#include <stdio.h>

void fillMatrixWithValue(int *M, int matrixSize, int value) {
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        M[i] = value;
    }
}

void fillMatrixWithIncrementValue(int *M, int matrixSize, int value, int step) {
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        M[i] = value;
        value += step;
    }
}

void fillData(int *A, int *B, int *C, int blockSize, int step) {
    int counter = 0;
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            counter += step;
            A[i * blockSize + j] = counter;
            B[i * blockSize + j] = 1;
            C[i * blockSize + j] = 0;
        }
    }
}

void printMatrix(int *M, int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            printf("%d\t", M[i * matrixSize + j]);
        }
        printf("\n");
    }
}

void multiply(int *a, int *b, int *c, int n) {
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++) {
                c[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}

void printMatrices(int *A, int *B, int *C, int blockSize) {
    printf("\nProcess 0: MATRIX A\n");
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            printf(" %d ", A[i * blockSize + j]);
        }
        printf("\n");
    }
    printf("\nProcess 0: MATRIX B\n");
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            printf(" %d ", B[i * blockSize + j]);
        }
        printf("\n");
    }
    printf("\nProcess 0: MATRIX C\n");
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            printf(" %d ", C[i * blockSize + j]);
        }
        printf("\n");
    }
}