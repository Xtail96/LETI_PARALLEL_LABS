#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]){
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
        //Действия, выполняемые только процессом с рангом 0
        printf ("Hello from main process %3d \n", ProcRank);

        for (int i = 1; i < ProcNum; i++)
        {
            //receive data (buffer, buffer size, buffer data type, sender number,message tag, group id,status)
            MPI_Recv(&RecvRank, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
            printf("Hello from process %3d \n", RecvRank);
        }
    }
    else
    {
        // Сообщение, отправляемое всеми процессами, кроме процесса с рангом 0

        //send data (buffer, buffer size, buffer data type, receiver process number, message tag, group id)
        MPI_Send(&ProcRank,1,MPI_INT,0,0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); //parallel part of app finish
    return 0;
}
