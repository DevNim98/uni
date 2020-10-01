#include <iostream>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono; 

mutex mu;

long long n;
double sum = 0.0;
int flag = 0;
int thread_count;

void BusyWaiting_pi(int rank)
{
    int my_rank = rank;
    double factor, my_sum = 0.0;
    long long my_n = n / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (long long i = my_first_i; i < my_last_i; i++, factor = -factor)
    {
        my_sum += factor / (2 * i + 1);
    }

    while (flag != my_rank);
    sum += my_sum;
    flag = (flag + 1) % thread_count;
}

void Mutex_pi(int rank)
{
    int my_rank = rank;
    double factor, my_sum = 0.0;
    long long my_n = n / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (long long i = my_first_i; i < my_last_i; i++, factor = -factor)
    {
        my_sum += factor / (2 * i + 1);
    }

    mu.lock();
    sum += my_sum;
    mu.unlock();
}

int main(int argc, char const *argv[])
{
    thread_count = atoi(argv[1]);
    n = strtoll(argv[2], NULL, 10);
    thread *threads_handler = new thread[thread_count];

    auto start = high_resolution_clock::now();
    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i] = thread(BusyWaiting_pi, i);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i].join();
    }
    auto finish = high_resolution_clock::now();
    auto elapsed = duration_cast<duration<double>>(finish - start);

    sum *= 4;

    cout << "Pi: " << setprecision(15) << sum << endl;
    cout << "Time elapsed bw: " << elapsed.count() << endl;

    sum = 0.0;

    start = high_resolution_clock::now();
    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i] = thread(Mutex_pi, i);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i].join();
    }
    finish = high_resolution_clock::now();
    elapsed = duration_cast<duration<double>>(finish - start);

    sum *= 4;

    cout << "Pi: " << setprecision(15) << sum << endl;
    cout << "Time elapsed mutex: " << elapsed.count() << endl;

    delete[] threads_handler;

    return 0;
}
