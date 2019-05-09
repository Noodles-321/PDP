#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MASTER 0 /* task ID of master task */

int partion(int R[], int start, int end)
{
    int r = R[start];
    while (start < end)
    {
        while ((R[end] > r) && (start < end))
            end--;
        R[start] = R[end];
        while ((R[start] < r) && (start < end))
            start++;
        R[end] = R[start];
    }
    R[start] = r;
    return start;
}

void quick_sort(int R[], int start, int end)
{
    int r;
    if (start < end)
    {
        r = partion(R, start, end);
        quick_sort(R, start, r - 1);
        quick_sort(R, r + 1, end);
    }
}

void merge_arrays(int arr1[], int n1, int arr2[], int n2, int arr3[])
{
    int i = 0, j = 0, k = 0;

    // Traverse both array
    while (i < n1 && j < n2)
    {
        // Check if current element of first
        // array is smaller than current element
        // of second array. If yes, store first
        // array element and increment first array
        // index. Otherwise do same with second array
        if (arr1[i] < arr2[j])
            arr3[k++] = arr1[i++];
        else
            arr3[k++] = arr2[j++];
    }

    // Store remaining elements of first array
    while (i < n1)
        arr3[k++] = arr1[i++];

    // Store remaining elements of second array
    while (j < n2)
        arr3[k++] = arr2[j++];
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Give 3 arguments: input_file_name, output_file_name, pivot_strategy_number(1-3):\n");
        return 0;
    }

    char *input_file_name = argv[1];
    char *output_file_name = argv[2];
    int pivot_strategy = atoi(argv[3]);

    int rank, size, rcode;
    int n;
    int *data = NULL;
    int chunk;             /* This many iterations will I do */
    int i, istart, istop;  /* Variables for the local loop   */
    double t_begin, t_end; //, time, t_total;
    // MPI_Status status;

    MPI_Init(&argc, &argv); /* Initialize MPI */
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */

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
        data = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
        {
            fscanf(f, "%d", &data[i]);
            //printf("%d\n", data[i]);
        }
        fclose(f);
    }
    // broadcast to other processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != MASTER)
    {
        data = (int *)malloc(n * sizeof(int));
    }
    MPI_Bcast(data, n, MPI_INT, 0, MPI_COMM_WORLD);

    chunk = n / size;               /* Number of numbers per processor */
    istart = rank * chunk;          /* Calculate start and stop indices  */
    istop = (rank + 1) * chunk - 1; /* for the local loop                */
    if (rank == size - 1)
        istop = n - 1; /* Make sure the last processor computes until the end    */

    t_begin = MPI_Wtime();

    quick_sort(data, istart, istop);
    //printf("qs end\n");
    //MPI_Comm group_comm[8];
    int group_rank, group_size;
    MPI_Comm group_comm;
    MPI_Comm last_comm = MPI_COMM_WORLD;
    group_rank = rank;
    group_size = size;

    // 新建局部数组
    int size_l = istop - istart + 1;
    int *local_array = NULL;
    local_array = (int *)malloc(size_l * sizeof(int));
    int *local_array_new = NULL; 
    memcpy(local_array, data + istart, size_l * sizeof(int));


    while (group_size > 1)
    {
        if (pivot_strategy == 1)
        {
            // Select the median in one processor in each group of processors.
            int p;
            if (group_rank == 0)
                p = local_array[size_l / 2];
            MPI_Bcast(&p, 1, MPI_INT, 0, last_comm);
        }
        else if (pivot_strategy == 2)
        {
            // Select the median of all medians in each processor group.
            int p = local_array[size_l / 2];
            if (group_rank != 0)
            {
                MPI_Send(&p, 1, MPI_INT, 0, 111, last_comm);
            }
            else
            {
                int *ps = NULL; // array to store medians
                ps = (int *)malloc(group_size * sizeof(int));
                ps[0] = p;
                for (int i = 1; i < size; i++)
                {
                    MPI_Recv(ps + i, 1, MPI_INT, i, 111, last_comm, &status);
                }
                quick_sort(ps, 0, group_size - 1);
                p = ps[group_size / 2]; // choose median
                MPI_Bcast(&p, 1, MPI_INT, 0, last_comm);
            }
        }
        else if (pivot_strategy == 3)
        {
            // Select the mean value of all medians in each processor group.
            int p = local_array[size_l / 2];
            if (group_rank != 0)
            {
                MPI_Send(&p, 1, MPI_INT, 0, 111, last_comm);
            }
            else
            {
                int *ps = NULL; // array to store medians
                ps = (int *)malloc(group_size * sizeof(int));
                ps[0] = p;
                for (int i = 1; i < size; i++)
                {
                    MPI_Recv(ps + i, 1, MPI_INT, i, 111, last_comm, &status);
                }
                int sum_median = 0;
                for (int i = 0; i < group_size; i++)
                {
                    sum_median += ps[i];
                }
                p = sum_median / group_size; // mean of medians
                MPI_Bcast(&p, 1, MPI_INT, 0, last_comm);
        }
        
        

        // find the spliting position
        int ip = size_l / 2;
        while (p < local_array[ip] && ip > 0)
            ip--;
        while (p > local_array[ip] && ip < size_l - 1)
            ip++;
        // ip is the index of the first number in each process larger than the pivot

        int color = group_rank % 2;
        int size_s, size_k, size_r;
        int *kept = NULL;
        int *received = NULL;

        if (color == 0)
        {
            size_s = size_l - ip;
            size_k = ip;
            // MPI_Send(buf, count, datatype, dest, tag, comm)
            MPI_Send(&size_s, 1, MPI_INT, group_rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&size_r, 1, MPI_INT, group_rank + 1, 0, MPI_COMM_WORLD, &status);
            received = (int *)malloc(size_r * sizeof(int));
            // send out the right large part, keep the left small part
            MPI_Send(local_array + ip, size_s, MPI_INT, group_rank + 1, 1, MPI_COMM_WORLD);
            // MPI_Recv(buf, count, datatype, source, tag, comm, status)
            // receive the small part
            MPI_Recv(received, size_r, MPI_INT, group_rank + 1, 1, MPI_COMM_WORLD, &status);
            kept = (int *)malloc(size_k * sizeof(int));
            memcpy(kept, local_array, size_k * sizeof(int));
        }
        if (color == 1)
        {
            size_s = ip;
            size_k = size_l - ip;

            // MPI_Send(buf, count, datatype, dest, tag, comm)
            MPI_Recv(&size_r, 1, MPI_INT, group_rank - 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&size_s, 1, MPI_INT, group_rank - 1, 0, MPI_COMM_WORLD);
            received = (int *)malloc(size_r * sizeof(int));
            // receive the large part
            // MPI_Recv(buf, count, datatype, source, tag, comm, status)
            MPI_Recv(received, size_r, MPI_INT, group_rank - 1, 1, MPI_COMM_WORLD, &status);
            // send out the left small part, keep the right large part
            MPI_Send(local_array, size_s, MPI_INT, group_rank - 1, 1, MPI_COMM_WORLD);
            kept = (int *)malloc(size_k * sizeof(int));
            memcpy(kept, local_array + ip, size_k * sizeof(int));
        }

        // 更新局部数组
        free(local_array);
        size_l = size_k + size_r; // local array size
        // int *local_array_new; 
        local_array_new = (int *)malloc(size_l * sizeof(int));
        merge_arrays(kept, size_k, received, size_r, local_array_new);

        free(received);
        free(kept);

        MPI_Comm_split(last_comm, color, group_rank, &group_comm);
        MPI_Comm_rank(group_comm, &group_rank);
        MPI_Comm_size(group_comm, &group_size);
        last_comm = group_comm;
    }

    // 数组拼接，覆盖data
    if (rank != MASTER)
    {
        MPI_Send(&size_l, 1, MPI_INT, MASTER, 999, MPI_COMM_WORLD);
        MPI_Send(local_array_new, size_l, MPI_INT, MASTER, 666, MPI_COMM_WORLD);
    }
    else
    {
        free(data);
        int *data = (int *)malloc(n * sizeof(int));
        if(size == 1)
          memcpy(data, local_array, size_l * sizeof(int));
        else
          memcpy(data, local_array_new, size_l * sizeof(int));
        
        int *local_array_sizes = NULL; 
        local_array_sizes = (int *)malloc(size * sizeof(int));
        int cum_size = size_l;

        for (int i = 1; i < size; i++)
        {
            MPI_Recv(local_array_sizes + i, 1, MPI_INT, i, 999, MPI_COMM_WORLD, &status);
            MPI_Recv(data + size_l, local_array_sizes[i], MPI_INT, i, 666, MPI_COMM_WORLD, &status);
            size_l += local_array_sizes[i];
        }
        free(local_array_sizes);
    }

    t_end = MPI_Wtime();

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
    }

    free(local_array_new);
    free(data);
    MPI_Finalize(); /* Shut down and clean up MPI */

    return 0;
}
