#include <semaphore.h>
#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;
using namespace std::chrono; 

int thread_count;
int counter = 0;
mutex barrier_mutex, cv_mutex;
sem_t barrier_sem;
sem_t count_sem;
condition_variable cond_var;

void Barrier_bsm(int rank)
{
    barrier_mutex.lock();
    counter++;
    barrier_mutex.unlock();
    while (counter < thread_count);
    if (rank == 0)
    {
        cout << "Todos los threads pasaron la barrera BWM" << endl;
    }
}

void Barrier_semaphores(int rank)
{
    sem_wait(&count_sem);
    if (counter == thread_count - 1)
    {
        counter = 0;
        sem_post(&count_sem);
        for (int j = 0; j < thread_count - 1; j++)
            sem_post(&barrier_sem);
    }
    else
    {
        counter++;
        sem_post(&count_sem);
        sem_wait(&barrier_sem);
    }
    if (rank == 0)
    {
        cout << "Todos los threads pasaron la barrera Semaphore" << endl;
    }
}

void Barrier_cv(int rank)
{
    unique_lock<mutex> lck(cv_mutex);
    counter++;
    if (counter == thread_count)
    {
        counter = 0;
        cond_var.notify_all();
    }
    else
    {
        cond_var.wait(lck);
    }
    
    if (rank == 0)
    {
        cout << "Todos los threads pasaron la barrera Condition variable" << endl;
    }
}

int main(int argc, char const *argv[])
{
    
    thread_count = atoi(argv[1]);
    thread *threads_handler = new thread[thread_count];

    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);

    auto start = high_resolution_clock::now();
    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i] = thread(Barrier_bsm, i);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i].join();
    }
    auto finish = high_resolution_clock::now();
    auto elapsed = duration_cast<duration<double>>(finish - start);

    cout << "Time elapsed bwm barrier: " << elapsed.count() << endl;
    counter = 0;

    start = high_resolution_clock::now();
    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i] = thread(Barrier_semaphores, i);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i].join();
    }
    finish = high_resolution_clock::now();
    elapsed = duration_cast<duration<double>>(finish - start);

    cout << "Time elapsed semaphore barrier: " << elapsed.count() << endl;
    counter = 0;

    start = high_resolution_clock::now();
    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i] = thread(Barrier_cv, i);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i].join();
    }
    finish = high_resolution_clock::now();
    elapsed = duration_cast<duration<double>>(finish - start);

    cout << "Time elapsed cv barrier: " << elapsed.count() << endl;

    sem_destroy(&count_sem);
    sem_destroy(&barrier_sem);
    delete[] threads_handler;

    return 0;
}
