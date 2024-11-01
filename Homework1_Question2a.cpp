/* Deveshwar Singh
 * CSCI 715 Distributed Systems
 * Homework 1 Question 2a: Write two parallel programs using Pthreads that each launch 7
                           threads saying “Hello from thread 1”, “Hello from thread 2”, etc. with the restriction
                           that: For the first program the threads must print their output in the following thread id order:
                           1,4,2,3,7,6,5
 */

#include <iostream>
#include <pthread.h>
#include <cstdlib>
using namespace std;

#define NUM_THREADS 7

pthread_mutex_t currentIndexMutex;
pthread_cond_t cond;

int order[] = {1, 4, 2, 3, 7, 6, 5};
int current_index = 0;

struct ThreadData {
    long threadId;
};

void* printThreads(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long threadId = data->threadId;

    pthread_mutex_lock(&currentIndexMutex);
    
    while(order[current_index] != threadId) {
        pthread_cond_wait(&cond, &currentIndexMutex);
    }

    cout<< "Hello from thread " << threadId << endl;

    current_index++;
    pthread_cond_broadcast(&cond);
    
    pthread_mutex_unlock(&currentIndexMutex);
    
    pthread_exit(0);
}

int main() {
    pthread_t workers[NUM_THREADS];
    pthread_attr_t attr;
    ThreadData threadData[NUM_THREADS];
    
    pthread_mutex_init(&currentIndexMutex, NULL);
    pthread_attr_init(&attr);
    pthread_cond_init(&cond, NULL);

    for (long i=0; i<NUM_THREADS; i++) {
        threadData[i].threadId = i + 1;
        pthread_create(&workers[i], &attr, printThreads, (void*)&threadData[i]);
    }

    for (long i=0; i<NUM_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }

    pthread_mutex_destroy(&currentIndexMutex);
    pthread_cond_destroy(&cond);
    pthread_attr_destroy(&attr);

    return 0;
}
