/* Deveshwar Singh
 * CSCI 715 Distributed Systems
 * Homework 1 Question 2b: Write two parallel programs using Pthreads that each launch 7
                           threads saying “Hello from thread 1”, “Hello from thread 2”, etc. with the restriction
                           that: For the second program threads 1 and 4 can print their output in any order and only
                           then threads 3,5,6,7 print their output also in any order. So for instance
                           4,1 | 7,6,3,5 and 1,4 | 3,5,7,6 are both valid orderings for output. But 7,1,5,6,3,4 is not.
 */

#include <iostream>
#include <pthread.h>
#include <cstdlib>
using namespace std;

#define NUM_THREADS 7

pthread_mutex_t sharedMutex;
pthread_cond_t cond;

int firstSetDone = 0;

struct ThreadData {
    long threadId;
};

void* printThreads(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long threadId = data->threadId;

    pthread_mutex_lock(&sharedMutex);
    
    if(threadId == 1 || threadId == 4) {
        cout<< "Hello from thread " << threadId << endl;
        
        firstSetDone++;
        if(firstSetDone == 2) {
            pthread_cond_broadcast(&cond);
        }
    }
    else {
        while(firstSetDone < 2) {
            pthread_cond_wait(&cond, &sharedMutex);
        }
        cout<< "Hello from thread " << threadId << endl;
    }

    pthread_mutex_unlock(&sharedMutex);
    
    pthread_exit(0);
}

int main() {
    pthread_t workers[NUM_THREADS];
    pthread_attr_t attr;
    ThreadData threadData[NUM_THREADS];
    
    pthread_mutex_init(&sharedMutex, NULL);
    pthread_attr_init(&attr);
    pthread_cond_init(&cond, NULL);

    for(long i=0; i<NUM_THREADS; i++) {
        threadData[i].threadId = i+1;
        pthread_create(&workers[i], &attr, printThreads, (void*)&threadData[i]);
    }

    for(long i=0; i<NUM_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }

    pthread_mutex_destroy(&sharedMutex);
    pthread_cond_destroy(&cond);
    pthread_attr_destroy(&attr);

    return 0;
}

