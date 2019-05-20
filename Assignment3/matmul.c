#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MASTER 0 /* task ID of master task */

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
    double *A = NULL, *B = NULL, *res = NULL;
    double *buffer_A = NULL, *buffer_B = NULL, *buffer_tmp = NULL;
    int chunk;             /* This many iterations will I do */
    double t_begin, t_end; //, time, t_total;
    // MPI_Status status;

    MPI_Init(&argc, &argv); /* Initialize MPI */
    MPI_Status status[40];
    MPI_Request req[40];
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
        if (n % size != 0)
        {
            printf("N can't be divied by number of processors!");
            exit(1);
        }
        A = (double *)malloc(n * n * sizeof(double));
        B = (double *)malloc(n * n * sizeof(double));
        //read A by rows
        for (int i = 0; i < n * n; i++)
        {
            fscanf(f, "%lf", &A[i]);
            //printf("%f\n", A[i]);
        }
        //read B by cols
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                fscanf(f, "%lf", &B[j * n + i]);
                //printf("%f\n", B[i]);
            }
        }
        fclose(f);
    }
    //IO finish, time begins
    t_begin = MPI_Wtime();

    MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    // broadcast to other processes
    chunk = n / size; /* Number of numbers per processor */
    buffer_A = (double *)malloc(chunk * n * sizeof(double));
    buffer_B = (double *)malloc(chunk * n * sizeof(double));
    //int MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
    //                void* recvbuf, int recvcount, MPI_Datatype recvtype,
    //                                int root, MPI_Comm comm)

    // partition by ROW
    MPI_Scatter(A, chunk * n, MPI_DOUBLE, buffer_A, chunk * n, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk * n, MPI_DOUBLE, buffer_B, chunk * n, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
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
    //double *buffer_C;
    buffer_tmp = (double *)malloc(chunk * n * sizeof(double));
    memcpy(buffer_tmp, buffer_B, chunk * n * sizeof(double));
    int left = ((rank - 1) % size + size) % size;
    int right = (rank + 1) % size;
    int col_bias = chunk * rank;
    res = (double *)malloc(chunk * n * sizeof(double));
    //buffer_C = (double *)malloc(chunk * n * sizeof(double));
    for (int i = 0; i < size; i++)
    {
        if (i > 0)
        {
            col_bias = ((left + 1) % size) * chunk;
        }
        for (int row = 0; row < chunk; row++)
        {
            for (int col = 0; col < chunk; col++)
            {
                double sum = 0.0;
                for (int k = 0; k < n; k++)
                {
                    sum += buffer_A[row * n + k] * buffer_tmp[col * n + k];
                }
                res[row * n + (col + col_bias)] = sum;
            }
        }
        if (i != size - 1)
        {
            // MPI_Send(buf, count, datatype, dest, tag, comm)
            MPI_Isend(buffer_B, chunk * n, MPI_DOUBLE, right, 666, MPI_COMM_WORLD, &req[rank]);
            MPI_Isend(&col_bias, 1, MPI_INT, right, 999, MPI_COMM_WORLD, &req[rank]);
            // MPI_Recv(buf, count, datatype, source, tag, comm, status)
            MPI_Recv(buffer_tmp, chunk * n, MPI_DOUBLE, left, 666, MPI_COMM_WORLD, &status[rank]);
            MPI_Recv(&col_bias, 1, MPI_INT, left, 999, MPI_COMM_WORLD, &status[rank]);
        }
        right = (right + 1) % size;
        left = ((left - 1) % size + size) % size;
    }
    double *fin_res = NULL;
    if (rank == MASTER)
        fin_res = (double *)malloc(n * n * sizeof(double));
    MPI_Gather(res, chunk * n, MPI_DOUBLE, fin_res, chunk * n, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
    t_end = MPI_Wtime();

    // Write data to file
    if (rank == MASTER)
    {
        printf("%.2f\n", t_end - t_begin);

        FILE *fp = fopen(output_file_name, "a");
        if (fp == NULL)
        {
            printf("File Error\n");
            exit(1);
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (j == n - 1)
                    fprintf(fp, "%f\n", fin_res[i * n + j]);
                else
                    fprintf(fp, "%f ", fin_res[i * n + j]);
            }
        }
        fclose(fp);
    }

    MPI_Finalize(); /* Shut down and clean up MPI */

    return 0;
}
