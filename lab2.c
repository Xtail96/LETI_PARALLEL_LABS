#include <stdlib.h>
#include <stdio.h>  
#include "mpi.h" 

int main(int argc, char* argv[])
{ 
    int ProcNum, ProcRank, RecvRank; 
    MPI_Status Status; 

    MPI_Init(&argc, &argv); //parallel part of app start
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); //declare size of processes (group id, group size(return))
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank); //define process rank in groud (group id, rank(return))

    if(ProcRank == 0)
    {
		int NextProcRank = rand() % (ProcNum - 1) + 1;
        printf("%3d throw ball to %3d \n\n", ProcRank, NextProcRank);
		MPI_Send(&ProcRank, 1, MPI_INT, NextProcRank, 0, MPI_COMM_WORLD);
        MPI_Recv(&RecvRank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);

		for(int i = 1; i < ProcNum; i++)
		{
	    	int stop = -1;
	    	MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
    }
    else
    {
		for(;;)
		{
		    MPI_Recv(&RecvRank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
		    if(RecvRank == -1)
		    {
		        break;
		    }

		    printf("%3d catch ball from %3d \n", ProcRank, RecvRank);

		    int NextProcRank = rand() % (ProcNum - 1);

		    if(NextProcRank == ProcRank)
		    {
				NextProcRank = NextProcRank + 1;
		    }
		    printf("%3d throw ball to %3d \n\n", ProcRank, NextProcRank);
		    
		    MPI_Send(&ProcRank, 1, MPI_INT, NextProcRank, 0, MPI_COMM_WORLD);
		}
    }

    MPI_Finalize(); //parallel part of app finish
    return 0; 
}