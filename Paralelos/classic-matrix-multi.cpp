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

    start = std::chrono::system_clock::now(); 
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            for (int k = 0; k < m; k++)
            {
                C[i][j] += A[i][k] * B[k][i];
            }
        }
    }

    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::time_t end_time = std::chrono::system_clock::to_time_t(end); 
  
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n"; 

    return 0;
}
