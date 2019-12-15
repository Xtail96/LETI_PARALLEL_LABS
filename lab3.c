#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int M = 5;
    int N = 6;
    int Matrix[M][N];
    for(int i = 0; i < M; i++)
    {
	   for(int j = 0; j < N; j++)
       {
	       Matrix[i][j] = rand() % 10;
	   }
    }
    int Result;

    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD,&ProcRank); 
    int Sum = 0;
    int Step = M*N/ProcNum;
    int* Pointer = (*Matrix + ProcRank*Step);
    int Num = Step;
    if(ProcRank == ProcNum - 1)
    {
        Num = Num + M*N % ProcNum;
    }

    for(int i = 0; i < Num; i++)
    {
	   Sum += Pointer[i];
    }

    MPI_Reduce(&Sum, &Result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (ProcRank == 0)
    {   
        printf("%3d\n", Result);    
    }

    MPI_Finalize();
    return 0;
}