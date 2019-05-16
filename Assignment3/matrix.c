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
    float *A = NULL, *B = NULL, *res = NULL;
    float *buffer_A = NULL, *buffer_B = NULL;
    int chunk;             /* This many iterations will I do */
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
        A = (float *)malloc(n * n * sizeof(float));
        B = (float *)malloc(n * n * sizeof(float));
        //read A by rows
        for (int i = 0; i < n * n; i++){
            fscanf(f, "%f", &A[i]);
            //printf("%f\n", A[i]);
        }
        //read B by cols
        for (int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
              fscanf(f, "%f", &B[j * n + i]);
              //printf("%f\n", B[i]);
            }
        }
        fclose(f);
    }
    //IO finish, time begins
    t_begin = MPI_Wtime();
  
    MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    // broadcast to other processes
    chunk = n / size;               /* Number of numbers per processor */
    buffer_A = (float *)malloc(chunk * n * sizeof(float));
    buffer_B = (float *)malloc(chunk * n * sizeof(float));
    //int MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
    //                void* recvbuf, int recvcount, MPI_Datatype recvtype,
    //                                int root, MPI_Comm comm)
    
    // partition by ROW
    MPI_Scatter(A, chunk * n, MPI_FLOAT, buffer_A, chunk * n, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk * n, MPI_FLOAT, buffer_B, chunk * n, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
    /* 
    for(int i = 0; i < size; i++){
      if(rank == i){
        for(int j = 0; j < chunk * n; j++){
          printf("%f and %f from %d\n", buffer_A[j], buffer_B[j], rank);
        }
      }
    } 
    */   
    free(A);
    free(B);
    float *buffer_C;
    left = (rank - 1) % size;
    right = (rank + 1) % size;
    buffer_C = (float *)malloc(chunk * n * sizeof(float));
    for(int i = 0; i < size; i++){
       for(int row = 0; row < chunk; row++){
           for(int col = 0; col < chunk; col++){
               int sum = 0;
               for(int k = 0; k < n; k++){
                 sum += buffer_A[row * n + k] * buffer_B[col * n + k];
               }
               buffer_C[row * n + col] = sum;
           }
       }
       if(i != size - 1)
           MPI_Send();
       if(i != size - 1)
           MPI_Recv();
    } 
        
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
