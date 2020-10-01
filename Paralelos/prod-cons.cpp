#include <semaphore.h>
#include <thread>
#include <iostream>
#include <string>

using namespace std;

int thread_count;
sem_t* semaphores;
string* msg;

void Send_msg(int rank) {
   int my_rank = rank;
   long dest = (my_rank + 1) % thread_count;

   string my_msg = "Hello to " + to_string(dest) + " from " + to_string(my_rank);
   msg[dest] = my_msg;
   sem_post(&semaphores[dest]);

   sem_wait(&semaphores[my_rank]);
   cout << "Thread " + to_string(my_rank) + " > " + msg[my_rank] << endl;
}

int main(int argc, char const *argv[])
{
    thread_count = atoi(argv[1]);
    thread *threads_handler = new thread[thread_count];
    semaphores = new sem_t[thread_count];
    msg = new string[thread_count];

    for (int i = 0; i < thread_count; i++)
    {
        sem_init(&semaphores[i], 0, 0);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i] = thread(Send_msg, i);
    }

    for (int i = 0; i < thread_count; i++)
    {
        threads_handler[i].join();
    }

    for (int i = 0; i < thread_count; i++) {
      sem_destroy(&semaphores[i]);
   }

    delete[] msg;
    delete[] threads_handler;
    delete[] semaphores;
    
    return 0;
}
