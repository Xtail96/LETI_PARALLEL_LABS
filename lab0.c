#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]){
    int ProcNum, ProcRank, RecvRank;
    int waiter = 0;
    MPI_Status Status;
    MPI_Init(&argc, &argv); //parallel part of app start
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); //declare size of processes (group id, group size(return))
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank); //define process rank in groud (group id, rank(return))
    if ( ProcRank == 0 ){ //Действия, выполняемые только процессом с рангом 0
        printf ("\n Hello from main process %3d", ProcRank);
    for ( int i=1; i<ProcNum; i++ ) {
            MPI_Recv(&RecvRank, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status); //receive data (buffer, buffer size, buffer data type, sender number,message id, group id,status)
        printf("\n Hello from process %3d", RecvRank);
    }
    } else // Сообщение, отправляемое всеми процессами, кроме процесса с рангом 0
    MPI_Send(&ProcRank,1,MPI_INT,0,0, MPI_COMM_WORLD);  //send data (buffer, buffer size, buffer data type, receiver process number, message idm group id)
    MPI_Finalize(); //parallel part of app finish
    return 0;
}
