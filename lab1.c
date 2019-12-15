#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

#define string char*

string makeString(const int len) {
    string s = malloc(4*sizeof(char));
    if (s == NULL) {
        printf("Allocation error.");
        exit(0);
    }

    return s;
}

string generateRandomString(const int len)
{
    static char avalableSymbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    string result = makeString(len);

    for(int i = 0; i < len; i++)
    {
        result[i] = '1';//avalableSymbols[rand() % (sizeof(avalableSymbols) - 1)]; 
    }
    return result;
}

void modifyString(string s) {
    static char avalableSymbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int position = rand() % sizeof(s);
    s[position] = avalableSymbols[rand() % (sizeof(avalableSymbols) - 1)];
}

int main(int argc, char* argv[])
{
    int len = 4;//rand() % 10;
    string originString = generateRandomString(len);
    printf("originString:  %3s \n", originString);

    int ProcNum, ProcRank, RecvRank;
    int waiter = 0;

    MPI_Status Status;
    MPI_Init(&argc, &argv); //parallel part of app start
    
    //declare size of processes (group id, group size(return))
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

    //define process rank in groud (group id, rank(return))
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if ( ProcRank == 0 )
    {
        // (buffer, buffer size, buffer data type, receiver process number, message id, group id)
        MPI_Send(originString, sizeof(originString), MPI_CHAR, 1, 0, MPI_COMM_WORLD);

        // (buffer, buffer size, buffer data type, sender number,message id, group id,status)
        MPI_Recv(originString, sizeof(originString), MPI_CHAR, ProcNum - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        printf("modifiedString:  %3s \n", originString);
    }
    else
    {
        MPI_Recv(originString, sizeof(originString), MPI_CHAR, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        
        int tmp = ProcRank + 1;
        printf("%s\n", originString);
        modifyString(originString);
        printf("%s\n", originString);
        
        tmp >= ProcNum ?
            MPI_Send(originString, sizeof(originString), MPI_CHAR, 0, 0, MPI_COMM_WORLD) :
            MPI_Send(originString, sizeof(originString), MPI_CHAR, tmp, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); //parallel part of app finish

    free(originString);
    return 0;
}
