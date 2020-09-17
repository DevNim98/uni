#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <mpi.h>

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

int FindPartner(int phase, int world_rank, int world_size)
{
  int partner;
  if (phase % 2 == 0)
    {
      if (world_rank % 2 != 0)
        partner = world_rank - 1;
      else
        partner = world_rank + 1;
    }
    else
    {
      if (world_rank % 2 != 0)
        partner = world_rank + 1;
      else
        partner = world_rank - 1;
    }
    if (partner == -1 || partner == world_size)
      partner = MPI_PROC_NULL;
    
    return partner;
}

void MergeLow(int my_keys[], int recv_keys[], int local_n)
{
  int m_i, r_i, t_i;
  m_i = r_i = t_i = 0;
  int *temp_keys = new int[local_n];

  while (t_i < local_n)
  {
    if (my_keys[m_i] <= recv_keys[r_i])
    {
      temp_keys[t_i] = my_keys[m_i];
      t_i++;
      m_i++;
    }
    else
    {
      temp_keys[t_i] = recv_keys[r_i];
      t_i++;
      r_i++;
    }
  }

  for (m_i = 0; m_i < local_n; m_i++)
    my_keys[m_i] = temp_keys[m_i];

  delete[] temp_keys;
}

void MergeHigh(int my_keys[], int recv_keys[], int local_n)
{
  int m_i, r_i, t_i;
  m_i = r_i = t_i = local_n - 1;
  
  int *temp_keys = new int[local_n];

  while (t_i >= 0)
  {
    if (my_keys[m_i] >= recv_keys[r_i])
    {
      temp_keys[t_i] = my_keys[m_i];
      t_i--;
      m_i--;
    }
    else
    {
      temp_keys[t_i] = recv_keys[r_i];
      t_i--;
      r_i--;
    }
  }

  for (m_i = 0; m_i < local_n; m_i++)
    my_keys[m_i] = temp_keys[m_i];

  delete[] temp_keys;
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

  int *global_keys, *local_keys, *recv_keys;
  string print = argv[1];
  int n = atoi(argv[2]);
  local_keys = new int[n / world_size];
  recv_keys = new int[n / world_size];

  if (world_rank == 0)
  {
    global_keys = RandomArray(n);
    cout << "Desordenado" << endl;
    if(print == "y")
      PrintArray(global_keys, n);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  local_start = MPI_Wtime();

  MPI_Scatter(global_keys, n / world_size, MPI_INT, local_keys, n / world_size, MPI_INT, 0, MPI_COMM_WORLD);
  sort(local_keys, local_keys + n / world_size);

  int partner;
  for (int phase = 0; phase < world_size; phase++)
  {    
    partner = FindPartner(phase, world_rank, world_size);
    if(partner != MPI_PROC_NULL)
    {
      MPI_Sendrecv(local_keys, n / world_size, MPI_INT, partner, 0,
                 recv_keys, n / world_size, MPI_INT, partner, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

      if (world_rank < partner)
        MergeLow(local_keys, recv_keys, n / world_size);
      else
        MergeHigh(local_keys, recv_keys, n / world_size);
    }
  }
  
  MPI_Gather(local_keys, n / world_size, MPI_INT, global_keys, n / world_size, MPI_INT, 0, MPI_COMM_WORLD);

  local_finish = MPI_Wtime();
  local_elapsed = local_finish - local_start;
  MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE,
  MPI_MAX, 0, MPI_COMM_WORLD);

  delete[] local_keys;
  delete[] recv_keys;

  if (world_rank == 0)
  {
    cout << "Ordenado" << endl;
    if(print == "y")
      PrintArray(global_keys, n);
    cout << "Elapsed time = " << elapsed << endl;
    delete[] global_keys;
  }

  MPI_Finalize();
  return 0;
}
