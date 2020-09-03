#include <stdlib.h>
#include <time.h>
#include <iostream>  
#include <chrono>  

int main(int argc, char const *argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end; 
    /* Initialize random seed */
    srand(time(NULL));

    const int m = 700;
    int A[m][m], B[m][m], C[m][m];

    // Initialize A, B and C
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            A[i][j] = rand() % m;
            B[i][j] = rand() % m;
            C[i][j] = 0;
        }
    }

    int i, j, k, kk, jj;
    double sum;
    int blockSize = 100;
    start = std::chrono::system_clock::now(); 
    for (kk = 0; kk < m; kk += blockSize)
    {
        for (jj = 0; jj < m; jj += blockSize)
        {
            for (i = 0; i < m; i++)
            {
                for (j = jj; j < jj + blockSize; j++)
                {
                    sum = C[i][j];
                    for (k = kk; k < kk + blockSize; k++)
                    {
                        sum += A[i][k] * B[k][j];
                    }
                    C[i][j] = sum;
                }
            }
        }
    }
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::time_t end_time = std::chrono::system_clock::to_time_t(end); 
  
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n"; 

    return 0;
}
