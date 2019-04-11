/**********************************************************************
 * This program evaluates an integral using MPI/C
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#define MASTER 0 /* task ID of master task */


int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Give 1 arguments: intervals\n");
    return 0;
  }
  int rank, size, rcode;
  // const long int intervals = 100000000L; /* The sum is [globally]
  //                                            divided into this many
  //                                            intervals     */
  const long int intervals = atol(argv[1]);
  int chunk;                             /* This many iterations will I do */
  int i, istart, istop;                  /* Variables for the local loop   */
  double ysum, xsum, dx, dy, xglobsum, yglobsum;
  double t_begin, t_end; //, time, t_total;

  MPI_Init(&argc, &argv); /* Initialize MPI */

  MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */
  t_begin = MPI_Wtime();

  chunk = intervals / size;   /* Number of intervals per processor */
  istart = rank * chunk + 1;  /* Calculate start and stop indices  */
  istop = (rank + 1) * chunk; /* for the local loop                */
  if (rank == size - 1)
  {
    istop = intervals; /* Make sure the last processor      */
  }                    /* computes until the end            */

  dx = 2.0 / intervals;
  dy = 2.0 / intervals;
  xsum = 0.0;
  ysum = 0.0;
  for (i = istart; i <= istop; i++)
  { /* The inner loop */
    double x = dx * (i - 1.0);
    double y = dy * (i - 1.0);
    xsum += dx * (x + sin(y) + 1);
  }
  /* COMPUTE THE INNER RESULT ADDING THE PARTIAL SUMS */
  MPI_Allreduce(&xsum, &xglobsum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  for (i = istart; i <= istop; i++)
  { /* The outer loop */
    double x = dx * (i - 1.0);
    double y = dy * (i - 1.0);
    ysum += dy * xglobsum;
  }
  MPI_Reduce(&ysum, &yglobsum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

  t_end = MPI_Wtime();
  // time = t_end - t_begin;
  // MPI_Reduce(&time, &t_total, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

  if (rank == MASTER)
  {
    // printf("%ld\t%.6f\t%.6f\n", intervals, yglobsum, t_end - t_begin);
    const char *output_file_name = "A1output";
    FILE *fp = fopen(output_file_name, "a");
    // if (fp == NULL)
    // {
    //   perror("Open file failed!");
    //   exit(1);
    // }
    fprintf(fp, "%ld\t%.6f\t%.6f\n", intervals, yglobsum, t_end - t_begin);
    // if (fclose(fp) != 0)
    // {
    //   perror("Close file failed!");
    //   exit(1);
    // }
    fclose(fp);
  }

  MPI_Finalize(); /* Shut down and clean up MPI */

  return 0;
}
