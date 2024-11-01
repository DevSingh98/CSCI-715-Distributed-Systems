/* Deveshwar Singh
 * CSCI 715 Distributed Systems
 * Homework 1 Question 3: Write a parallel program using Pthreads that implements even-odd
                          sort on a given array of integers.
 
 Note: I wanted to use pthread_barrier_t but thats only supported on Linux and not macOS so I mimicked a barrier with a mutex, conditional variable and a counter just incase so it runs on whatever OS you are using.
 
 */

#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
using namespace std;

#define NUM_THREADS 4
#define SIZE 100

struct ThreadData {
    int threadId;
    int *arr;
    int sectionSize;
};

bool sorted = false;
bool swapped = false;
pthread_mutex_t sortedMutex;

pthread_mutex_t barrierMutex;
pthread_cond_t barrierCond;
int barrierCount = 0;

void barrierWait(int numThreads) {
    pthread_mutex_lock(&barrierMutex);
    barrierCount++;
    if (barrierCount == numThreads) {
        barrierCount = 0;
        pthread_cond_broadcast(&barrierCond);
    } else {
        pthread_cond_wait(&barrierCond, &barrierMutex);
    }
    pthread_mutex_unlock(&barrierMutex);
}

void* evenOddSort(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int threadId = data->threadId;
    int* arr = data->arr;
    int sectionSize = data->sectionSize;

    int start = threadId * sectionSize;
    int end = start + sectionSize;

    if (threadId == NUM_THREADS-1) {
        end = SIZE;
    }

    do{
        if (threadId == 0) {
            pthread_mutex_lock(&sortedMutex);
            swapped = false;
            pthread_mutex_unlock(&sortedMutex);
        }
        
        barrierWait(NUM_THREADS);

        bool swapped = false;
        for(int i=start; i<end-1; i+=2) {
            if (arr[i] > arr[i+1]) {
                int temp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = temp;
                swapped = true;
            }
        }

        barrierWait(NUM_THREADS);

        for(int i = start+1; i<end-1; i+=2) {
            if (arr[i] > arr[i+1]) {
                swap(arr[i], arr[i+1]);
                swapped = true;
            }
        }

        if(end<SIZE && arr[end-1] > arr[end]) {
            swap(arr[end-1], arr[end]);
            swapped = true;
        }

        barrierWait(NUM_THREADS);

        if(swapped) {
            pthread_mutex_lock(&sortedMutex);
            swapped = true;
            pthread_mutex_unlock(&sortedMutex);
        }

        barrierWait(NUM_THREADS);

        if(threadId == 0) {
            pthread_mutex_lock(&sortedMutex);
            sorted = !swapped;
            pthread_mutex_unlock(&sortedMutex);
        }

        barrierWait(NUM_THREADS);

    } while(!sorted);

    pthread_exit(0);
}

void fillUpArr(int *input, int size) {
    srand(static_cast<unsigned int>(time(0)));
    for(int i = 0; i < size; i++)
    input[i] = rand() % 1000;
}

void printArray(int arr[], bool isSorted) {
    string temp = (isSorted) ? "Sorted array: " : "Unsorted array: ";
    cout<< temp;
    for(int i = 0; i < SIZE; ++i){
        cout<< arr[i] << " ";
    }
    cout<< endl << endl;
}

int main() {
    int arr[SIZE];
    fillUpArr(arr, SIZE);

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int sectionSize = SIZE / NUM_THREADS;

    pthread_mutex_init(&sortedMutex, NULL);
    pthread_mutex_init(&barrierMutex, NULL);
    pthread_cond_init(&barrierCond, NULL);

    printArray(arr, false);

    for(int i=0; i<NUM_THREADS; i++) {
        threadData[i].threadId = i;
        threadData[i].arr = arr;
        threadData[i].sectionSize = sectionSize;
        pthread_create(&threads[i], NULL, evenOddSort, (void*)&threadData[i]);
    }

    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printArray(arr, true);
    pthread_mutex_destroy(&sortedMutex);
    pthread_mutex_destroy(&barrierMutex);
    pthread_cond_destroy(&barrierCond);

    return 0;
}

