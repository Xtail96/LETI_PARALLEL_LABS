#include <mpi.h>
#include <iostream>

#include "matrix_functions.h"

int ProcNum; 
int ProcRank;
int debug_info=0;
int Size;
double *A;
double *B;
double *C;

double matrixMultiplicationParallelRibbon(double *A, double *B,  double *C, int size){
    int i = 0, j = 0;

    double *bufA, *bufB, *bufC;
    int dim = size;

    MPI_Status Status;

    int ProcPartsize = dim/ProcNum;
    int ProcPartElem = ProcPartsize*dim;

    bufA = new double[ProcPartElem];
    bufB = new double[ProcPartElem];
    bufC = new double[ProcPartElem];

    for (i = 0; i < ProcPartElem; i++)
    {
        bufC[i] = 0;
    }
        
    double time_start = MPI_Wtime();
    MPI_Scatter(A, ProcPartElem, MPI_DOUBLE, bufA, ProcPartElem, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, ProcPartElem, MPI_DOUBLE, bufB, ProcPartElem, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int k = 0 ;
    int NextProc = ProcRank + 1;
    if ( ProcRank == ProcNum - 1 ) NextProc = 0;

    int PrevProc = ProcRank - 1;
    if ( ProcRank == 0 ) PrevProc = ProcNum - 1;

    int PrevDataNum = ProcRank;
    for (int p = 0; p < ProcNum; p++)
    {
        for (i = 0; i < ProcPartsize; i++)
        {
            for (j = 0; j < size; j++)
            {
                double tmp = 0;
                for (k = 0; k < ProcPartsize; k++)
                    tmp += bufA[PrevDataNum * ProcPartsize + i * size + k] * bufB[k * size + j];
                bufC[i * size + j] += tmp;
            }
        }

        PrevDataNum -= 1;

        if (PrevDataNum < 0)
            PrevDataNum = ProcNum - 1;

        MPI_Sendrecv_replace(bufB, ProcPartElem, MPI_DOUBLE, NextProc, 0, PrevProc, 0, MPI_COMM_WORLD, &Status);
    }

    MPI_Gather(bufC, ProcPartElem, MPI_DOUBLE, C, ProcPartElem, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    double time_finish = MPI_Wtime();
    return time_finish - time_start;
}

void InitProcess (double* &A,double* &B,double* &C ,int &Size) {
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0) {
        Size = ProcNum;
    }

    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (ProcRank == 0) {
        A = new double [Size*Size];
        B = new double [Size*Size];
        C = new double [Size*Size];
        RandInit (A, Size); RandInit (B, Size);
    }
}


int main(int argc, char **argv) {
    double beg, end, serial;

    MPI_Init ( &argc, &argv );  
    InitProcess (A,B,C,Size);
    
    double parallel_latency = matrixMultiplicationParallelRibbon(A,B,C,Size);

    if (ProcRank == 0) {
        std::cout << std::fixed << std::endl << "Parallel latency = " << parallel_latency << std::endl;
         
        double sequential_latency = matrixMultiplicationSequential(A, B, C, Size);
        std::cout << std::fixed << std::endl << "Sequential latency = " << sequential_latency << std::endl;
        
        if (debug_info) {
            printMatrices(A, B, C, Size);
        }   
    }
    MPI_Finalize();
    delete [] A;
    delete [] B;
    delete [] C;  
}
