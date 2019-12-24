#include <mpi.h>
//#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
//#include <curses.h>
//
#include <math.h>
#include <ctime>
#include <chrono>
int ProcNum; 
int ProcRank;
int flag=0;
int Size;
double *A;  double *B; double *C;

//***********************************************************
void PrintMatrix(double* pMatrix,int Size) {
    for (int i=0;i<Size;i++) {printf("\n");
        for (int j=0;j<Size;j++)
            printf("%7.4f ",pMatrix[i*Size+j]);
    }
}
//------------------------------------------------------------
void RandInit (double* pMatrix, int Size) {
    srand(100);
    for (int i=0; i<Size; i++) {
        for (int j=0;j<Size;j++)  pMatrix[i*Size+j]=rand()/double(1000);
    }
}
//-------------------------------------------------
void InitProcess (double* &A,double* &B,double* &C ,int &Size) {
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0) {
        Size = ProcNum;
    }
    if (Size<10) flag=1;
    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (ProcRank == 0) {
        A = new double [Size*Size];
        B = new double [Size*Size];
        C = new double [Size*Size];
        RandInit (A, Size); RandInit (B, Size);
    }
}
//-------------------------------------------------
void Flip (double *&B, int dim) {
    double temp=0.0;
    for (int i=0; i<dim; i++){
        for (int j=i+1; j<dim; j++){
            temp=B[i*dim+j];
            B[i*dim+j]=B[j*dim+i];
            B[j*dim+i]=temp;
        }
    }
}
//-------------------------------------------------
void MatrixMultiplicationMPI(double *&A, double *&B, double *&C, int &Size) {
    int dim =Size;
    int i, j, k, p, ind;
    double temp;
    MPI_Status Status;
    int ProcPartSize=dim/ProcNum;
    int ProcPartElem=ProcPartSize*dim;
    double* bufA=new double[dim*ProcPartSize];
    double* bufB=new double[dim*ProcPartSize];
    double* bufC=new double[dim*ProcPartSize];
    int ProcPart = dim/ProcNum, part = ProcPart*dim;
    if (ProcRank == 0) {
        Flip(B,Size);
    }
    
    MPI_Scatter(A, part, MPI_DOUBLE, bufA, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, part, MPI_DOUBLE, bufB, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    temp=0.0;
    for(i=0;i<ProcPartSize;i++) {
        for(j=0;j<ProcPartSize; j++) {
            for(k=0;k<dim;k++) 
                temp+=bufA[i*dim+k]*bufB[j*dim+k];
            bufC[i*dim+j+ProcPartSize*ProcRank]=temp;
            temp=0.0;
        }
    }
    
    int NextProc; int PrevProc;
    for(p=1; p<ProcNum; p++) {
        NextProc = ProcRank+1;
        if ( ProcRank ==ProcNum-1 ) NextProc = 0;
        PrevProc = ProcRank - 1;
        if ( ProcRank == 0 ) PrevProc = ProcNum-1;
        MPI_Sendrecv_replace(bufB,part, MPI_DOUBLE, NextProc, 0, PrevProc, 0, MPI_COMM_WORLD, &Status);
        temp=0.0;
        for (i=0; i<ProcPartSize; i++) {
            for(j=0; j<ProcPartSize; j++) {
                for(k=0; k<dim; k++){
                    temp +=bufA[i*dim+k]*bufB[j*dim+k];}
                if (ProcRank - p >= 0 )
                    ind=ProcRank - p;
                else ind=(ProcNum - p+ProcRank);
                bufC[i*dim+j+ind*ProcPartSize]=temp;
                temp=0.0;
            }
        }
    }
    MPI_Gather(bufC, ProcPartElem, MPI_DOUBLE, C, ProcPartElem, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    delete []bufA;
    delete []bufB;
    delete []bufC;
}

void MatrixMultiplicationMPI2(double *A, double *B,  double *C,int size){

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

      bufC[i] = 0;

   MPI_Scatter(A, ProcPartElem, MPI_DOUBLE, bufA, ProcPartElem, MPI_DOUBLE, 

               0, MPI_COMM_WORLD);

   MPI_Scatter(B, ProcPartElem, MPI_DOUBLE, bufB, ProcPartElem, MPI_DOUBLE,

               0, MPI_COMM_WORLD);

   int k = 0 ;

 

   int NextProc = ProcRank + 1;

   if ( ProcRank == ProcNum - 1 ) NextProc = 0;

   int PrevProc = ProcRank - 1;

   if ( ProcRank == 0 ) PrevProc = ProcNum - 1;

 

   int PrevDataNum = ProcRank;

 

   for (int p = 0; p < ProcNum; p++)

   {

      for (i = 0; i < ProcPartsize; i++)

         for (j = 0; j < size; j++)

         {

            double tmp = 0;

                  for (k = 0; k < ProcPartsize; k++)

                        tmp += bufA[PrevDataNum * ProcPartsize + i * size + k] * bufB[k * size + j];

              bufC[i * size + j] += tmp;

            }

     

      PrevDataNum -= 1;

      if (PrevDataNum < 0)

              PrevDataNum = ProcNum - 1;

      MPI_Sendrecv_replace(bufB, ProcPartElem, MPI_DOUBLE, NextProc, 0, PrevProc, 0, MPI_COMM_WORLD, &Status);

   }

   MPI_Gather(bufC, ProcPartElem, MPI_DOUBLE, C, ProcPartElem, MPI_DOUBLE, 0, MPI_COMM_WORLD);

}

//--------------------------------------------------------

int main(int argc, char **argv) {
    double beg, end, serial, parallel, notparallel=0;
    MPI_Init ( &argc, &argv );  
    InitProcess (A,B,C,Size);
    beg=MPI_Wtime();
    MatrixMultiplicationMPI(A,B,C,Size);
    end=MPI_Wtime(); 
    parallel= end-beg; 

    if (ProcRank == 0) {
        // printf("\n",&ProcNum);
        printf ("\n");
        printf("\nTime of execution -  Parallel calculation:\n");
        printf("%7.4f",parallel);
        
        auto non_paral_beg=std::chrono::system_clock::now();
        
        for (int i=0; i<Size; i++){
            for (int j=0; j<Size;j++) {
                C[i*Size+j]=0;
                for (int k=0; k<Size; k++){
                    C[i*Size+j] += A[i*Size+k]*B[j*Size+k];
                }
            }
        }
        
        auto non_paral_end=std::chrono::system_clock::now(); 
        notparallel= std::chrono::duration<double>(end-beg).count(); 
        printf("\nTime of execution -  Not Parallel calculation:\n");
        printf("%7.4f",notparallel);
        
        if (flag) {
            printf("\nMatrix C - Parallel calculation\n");
            PrintMatrix(C,Size);
        }   
    }
    MPI_Finalize();
    delete [] A; delete [] B; delete [] C;  
    // getch();
    
}