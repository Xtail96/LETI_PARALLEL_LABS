#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>
#include "matrix_functions.h"

#define false 0

int main(int argc, char *argv[]) {
    // Бесполезные переменные
    int rank_2d;

    // Переменные MPI
    MPI_Status status;
    MPI_Comm CartesianCommunicator;
    MPI_Request requestA0, requestA1, requestB0, requestB1;
    MPI_Datatype MatrixBlockType, ScatterType1, ScatterType2;
    int ProcNum, ProcRank;
    int processCoords[2];

    // Остальные переменные
    int i = 0, j = 0, k = 0, *A, *B, *C, n;
    char ch;
    int *buffA0, *buffA1, *buffB0, *buffB1;
    int LS, RS, US, DS;

    int dimensions[2];
    int periodic[2] = {1, 1};

    int matrix_size, block_size;
    double startTime, endTime, deltaTime;
    unsigned long long int startRTSC, endRTSC;
    unsigned int deltaRTSC;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int communicator_dimension = sqrt(ProcNum);

    if (pow(communicator_dimension, 2) != ProcNum) {
        if (ProcRank == 0)
            fprintf(stderr, "Программа работает при количестве процессов равном квадрату натурального числа\n");
        MPI_Finalize();
        return 0;
    }

    dimensions[0] = dimensions[1] = communicator_dimension;
    if (ProcRank == 0) {
        printf("Количество потоков: %d\nФормат коммуникатора(NxN): %dx%d\n", ProcNum, communicator_dimension, communicator_dimension);

        do {
            printf("Количество строк(M) матрицы, должно быть кратно размеру коммуникатора(N): M mod N = 0\n");
            printf("Введите dim:\n");
            scanf("%d", &matrix_size);
            printf("Количество строк матрицы: %d, количество строк коммуникатора: %d\n", matrix_size, communicator_dimension);
        } while (matrix_size % communicator_dimension != 0);

        startRTSC = _rdtsc();
        startTime = MPI_Wtime();
    }

    MPI_Bcast(&matrix_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    block_size = matrix_size / communicator_dimension; // small block dimension

    MPI_Type_contiguous(block_size * block_size, MPI_INT, &MatrixBlockType); // create matrix datatype to transfer
    MPI_Type_commit(&MatrixBlockType);

//    MPI_Type_vector(2, 2, 4, MPI_INT, &ScatterType1); // for the MPI_Scatterv
//    MPI_Type_create_resized(ScatterType1, 0, 2 * sizeof(int), &ScatterType2);
//    MPI_Type_commit(&ScatterType2);

    // each process has matrix A & B & C
//    A = (int *) malloc(block_size * block_size * sizeof(int *));
//    B = (int *) malloc(block_size * block_size * sizeof(int *));
//    C = (int *) malloc(block_size * block_size * sizeof(int *));
    A = (int *) malloc(block_size * block_size * sizeof(int));
    B = (int *) malloc(block_size * block_size * sizeof(int));
    C = (int *) malloc(block_size * block_size * sizeof(int));

//  MPI_Cart_create(MPI_Comm old, int ndims, const int dims[], const int periods[], int reorder, MPI_Comm * comm_cart) returns int
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, periodic, false, &CartesianCommunicator);
//  MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int coords[]) returns int
    MPI_Cart_coords(CartesianCommunicator, ProcRank, 2, processCoords);
//  MPI_Cart_rank(MPI_Comm comm, const int coords[], int *rank) returns int
    MPI_Cart_rank(CartesianCommunicator, processCoords, &rank_2d);

    fillMatrixWithIncrementValue(A, block_size, 1, 1);
    fillMatrixWithIncrementValue(B, block_size, 8, -1);
    fillMatrixWithValue(C, block_size, 0);

    buffA0 = malloc(sizeof(int) * block_size * block_size);
    buffA1 = malloc(sizeof(int) * block_size * block_size);
    buffB0 = malloc(sizeof(int) * block_size * block_size);
    buffB1 = malloc(sizeof(int) * block_size * block_size);

    MPI_Cart_shift(CartesianCommunicator, 1, processCoords[0], &LS, &RS); // to know the left and right neighbors
    MPI_Cart_shift(CartesianCommunicator, 0, processCoords[1], &US, &DS); // to know the top and bottom neighbors

//    printf("(%d , %d) => (l: %d, r: %d)\n", processCoords[0], processCoords[1], LS, RS);

    for (int l = 0; l < block_size; l++) {
        for (int o = 0; o < block_size; o++) {
            buffA0[l * block_size + o] = A[l * block_size + o];
            buffB0[l * block_size + o] = B[l * block_size + o];
        }
    }

    MPI_Isend(buffA0, 1, MatrixBlockType, LS, 1, CartesianCommunicator, &requestA0);
    MPI_Irecv(buffA1, 1, MatrixBlockType, RS, 1, CartesianCommunicator, &requestA1);

    MPI_Isend(buffB0, 1, MatrixBlockType, US, 1, CartesianCommunicator, &requestB0);
    MPI_Irecv(buffB1, 1, MatrixBlockType, DS, 1, CartesianCommunicator, &requestB1);

    MPI_Wait(&requestA0, &status);
    MPI_Wait(&requestA1, &status);
    MPI_Wait(&requestB0, &status);
    MPI_Wait(&requestB1, &status);

    for (int l = 0; l < block_size; l++) {
        for (int o = 0; o < block_size; o++) {
            A[l * block_size + o] = buffA1[l * block_size + o];
            B[l * block_size + o] = buffB1[l * block_size + o];
        }
    }

    matrixMultiplicationSequential(A, B, C, block_size);

    for (i = 1; i < communicator_dimension; i++) {
        // Получение номеров соседей
        MPI_Cart_shift(CartesianCommunicator, 1, 1, &LS, &RS);
        MPI_Cart_shift(CartesianCommunicator, 0, 1, &US, &DS);

        for (int l = 0; l < block_size; l++) {
            for (int o = 0; o < block_size; o++) {
                buffA0[l * block_size + o] = A[l * block_size + o];
            }
        }
        for (int l = 0; l < block_size; l++) {
            for (int o = 0; o < block_size; o++) {
                buffB0[l * block_size + o] = B[l * block_size + o];
            }
        }
        //Осуществление сдвига
        MPI_Isend(buffA0, 1, MatrixBlockType, LS, 1, CartesianCommunicator, &requestA0);
        MPI_Irecv(buffA1, 1, MatrixBlockType, RS, 1, CartesianCommunicator, &requestA1);

        MPI_Isend(buffB0, 1, MatrixBlockType, US, 1, CartesianCommunicator, &requestB0);
        MPI_Irecv(buffB1, 1, MatrixBlockType, DS, 1, CartesianCommunicator, &requestB1);

        MPI_Wait(&requestA0, &status);
        MPI_Wait(&requestA1, &status);
        MPI_Wait(&requestB0, &status);
        MPI_Wait(&requestB1, &status);

        for (int l = 0; l < block_size; l++) {
            for (int o = 0; o < block_size; o++) {
                A[l * block_size + o] = buffA1[l * block_size + o];
                B[l * block_size + o] = buffB1[l * block_size + o];
            }
        }
        matrixMultiplicationSequential(A, B, C, block_size);
    }

    if (ProcRank == 0) {
        endRTSC = _rdtsc();
        endTime = MPI_Wtime();

        if (startRTSC < endRTSC)
            deltaRTSC = endRTSC - startRTSC;
        else
            deltaRTSC = startRTSC - endRTSC;
        deltaTime = endTime - startTime;

        printf("Start RTSC:\t%llu\n"
               "End RTSC:\t%llu\n"
               "Delta RTSC:\t%llu\n"
               "Общее время вычислений:\t%f\n", startRTSC, endRTSC, deltaRTSC, deltaTime);
    }

    MPI_Finalize();
    return 0;
}
