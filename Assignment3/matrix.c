#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MASTER 0 /* task ID of master task */
int* matrix_mul(int *A, int *B){
}

int* matrix_add(int *A, int *B){
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Give 2 arguments: input_file_name, output_file_name:\n");
        return -1;
    }

    char *input_file_name = argv[1];
    char *output_file_name = argv[2];

    int n, rank, size;
    int *A = NULL, *B = NULL, *res = NULL;
    int *buffer_A = NULL, *buffer_B = NULL;
    int chunk;             /* This many iterations will I do */
    int i, istart, istop;  /* Variables for the local loop   */
    double t_begin, t_end; //, time, t_total;
    // MPI_Status status;

    MPI_Init(&argc, &argv); /* Initialize MPI */
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my rank                  */

    // Read data from file
    // read in process 0
    if (rank == MASTER)
    {
        FILE *f;
        f = fopen(input_file_name, "r");
        if (f == NULL)
        {
            printf("File Error\n");
            exit(1);
        }
        fscanf(f, "%d", &n);
        //printf("%d\n", n);
        if(n % size != 0){
          printf("N can't be divied by number of processors!");
          exit(1);
        }
        A = (int *)malloc(n * n * sizeof(int));
        B = (int *)malloc(n * n * sizeof(int));
        for (int i = 0; i < n * n; i++){
            fscanf(f, "%d", &A[i]);
            //printf("%d\n", A[i]);
        }
        for (int i = 0; i < n * n; i++){
            fscanf(f, "%d", &B[i]);
            //printf("%d\n", B[i]);
        }
        fclose(f);
    }
    MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    t_begin = MPI_Wtime();
    // broadcast to other processes
    chunk = n / size;               /* Number of numbers per processor */
    //istart = rank * chunk;          /* Calculate start and stop indices  */
    //istop = (rank + 1) * chunk - 1; /* for the local loop                */
    buffer_A = (int *)malloc(chunk * n * sizeof(int));
    buffer_B = (int *)malloc(chunk * n * sizeof(int));
    //int MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
    //                void* recvbuf, int recvcount, MPI_Datatype recvtype,
    //                                int root, MPI_Comm comm)
    
    // partition by ROW
    MPI_Scatter(A, chunk * n, MPI_INT, buffer_A, chunk * n, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk * n, MPI_INT, buffer_B, chunk * n, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    for(int i = 0; i < size; i++){
      if(rank == i){
        for(int j = 0; j < chunk * n; j++){
          printf("%d and %d from %d\n", buffer_A[j], buffer_B[j], rank);
        }
      }
    }    
    free(A);
    free(B);
    //memcpy(local_array, data + istart, size_l * sizeof(int));


            // MPI_Send(buf, count, datatype, dest, tag, comm)
            // MPI_Recv(buf, count, datatype, source, tag, comm, status)


    t_end = MPI_Wtime();
    /*
    // Write data to file
    if (rank == MASTER)
    {
        printf("%.2f\n", t_end - t_begin);
        
        for (int i = 0; i < n - 1; i++)
        {
            if (data[i] > data[i + 1])
            {
                printf("Wrong!\n");
                break;
            }
        }

        FILE *fp = fopen(output_file_name, "a");
        if (fp == NULL)
        {
            printf("File Error\n");
            exit(1);
        }
        for (int i = 0; i < n; i++)
        {
            fprintf(fp, "%d  ", data[i]);
        }

        fclose(fp);
    }*/
    MPI_Finalize(); /* Shut down and clean up MPI */

    return 0;
}
