#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <string>

using namespace std;

int *RandomArray(int count)
{
  int *result = new int[count];
  for (int i = 0; i < count; i++)
  {
    result[i] = rand() % count + 1;
  }
  return result;
}

int *MatrixVectorMultiplication(int A[], int x[], int m, int n)
{
  int *y = new int[m];
  for (int i = 0; i < m; i++)
  {
    y[i] = 0;
    for (int j = 0; j < n; j++)
    {
      y[i] += A[i * n + j] * x[j];
    }
  }
  return y;
}

void PrintArray(int data[], int count)
{
  for (int i = 0; i < count; i++)
  {
    cout << data[i] << " ";
  }
  cout << endl;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  MPI_Init(&argc, &argv);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  double local_start, local_finish, local_elapsed, elapsed;
  int m, n;
  int *A, *x, *y;
  int *local_A, *local_y;

  string print = argv[1];
  m = atoi(argv[2]);
  n = atoi(argv[3]);

  if (world_rank == 0)
  {
    A = RandomArray(m * n);
    x = RandomArray(n);
    y = new int[m];
  }
  else
  {
    x = new int[n];
  }

  local_A = new int[m / world_size * n];
  local_y = new int[m / world_size * n];

  MPI_Barrier(MPI_COMM_WORLD);
  local_start = MPI_Wtime();

  MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(A, m / world_size * n, MPI_INT, local_A,
              m / world_size * n, MPI_INT, 0, MPI_COMM_WORLD);
  
  local_y = MatrixVectorMultiplication(local_A, x, m / world_size, n);

  MPI_Gather(local_y, m / world_size, MPI_INT, y, m / world_size, MPI_INT, 0, MPI_COMM_WORLD);

  local_finish = MPI_Wtime();
  local_elapsed = local_finish - local_start;
  MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE,
  MPI_MAX, 0, MPI_COMM_WORLD);
  
  if (world_rank == 0)
  {
    if(print == "yes")
    {
      cout << "Matriz A:" << endl;
      for (int i = 0; i < m; i++)
      {
        PrintArray(&A[i * n], n);
      }

      cout << "Vector x:" << endl;
      PrintArray(x, n);

      cout << "Vector y:" << endl;
      PrintArray(y, m);
    }    
    cout << "Elapsed time = " << elapsed << endl;
    
    delete[] A;
    delete[] y;
  }

  delete[] local_A;
  delete[] x;
  delete[] local_y;
  MPI_Finalize();
  return 0;
}
