#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

#define string char*

string makeString(const int len) {
    string s = (char*) malloc(len*sizeof(char));
    if (s == NULL) {
        printf("Allocation error.");
        MPI_Finalize();
        exit(0);
    }

    for(int i = 0; i < len; i++)
    {
        s[i] = '0'; 
    }

    return s;
}

int main(int argc, char* argv[])
{
    int ProcNum, ProcRank, RecvRank;
    int waiter = 0;

    MPI_Status Status;
    MPI_Init(&argc, &argv);
    
    //declare size of processes (group id, group size(return))
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

    int len = 100;
    string str = makeString(len);

    int rands[ProcNum];
    for(int i = 0; i < ProcNum; i++)
    {
        rands[i] = rand() % (len - 1);
    }

    //define process rank in groud (group id, rank(return))
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if ( ProcRank == 0 )
    {
        // (buffer, buffer size, buffer data type, receiver process number, message tag, group id)
        MPI_Send(str, len, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

        // (buffer, buffer size, buffer data type, sender number,message tag, group id,status)
        MPI_Recv(str, len, MPI_CHAR, ProcNum - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        printf("modifiedString:  %3s \n", str);
    }
    else
    {
        MPI_Recv(str, len, MPI_CHAR, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        
        int tmp = ProcRank + 1;
        str[rands[ProcRank - 1]] = '1';
        
        tmp >= ProcNum ?
            MPI_Send(str, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD) :
            MPI_Send(str, len, MPI_CHAR, tmp, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); //parallel part of app finish

    free(str);
    return 0;
}
