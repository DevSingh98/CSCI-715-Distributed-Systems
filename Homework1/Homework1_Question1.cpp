/* Deveshwar Singh
 * CSCI 715 Distributed Systems
 * Homework 1 Question 1: Write a parallel program using Pthreads that is able to determine if an array of integers is sorted.
 */

#include <iostream>
#include <pthread.h>
#include <cstdlib>
using namespace std;

#define NUM_THREADS 4
#define SIZE 100

int arr[SIZE];
bool isSorted = true;
pthread_mutex_t isSortedMutex;

struct ThreadData {
    int start;
    int end;
};


void* checkSort(void* arg){
    ThreadData* data = (ThreadData*)arg;
    int start = data->start;
    int end = data->end;

    for(int i=start; i<end-1; ++i) {
        if (arr[i] > arr[i+1]) {
            pthread_mutex_lock(&isSortedMutex);
            isSorted= false;
            pthread_mutex_unlock(&isSortedMutex);
            pthread_exit(0);
        }
    }
    pthread_exit(0);
}

void fillUpArr(int* input, int size) {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i<size; i++)
    input[i] = rand() % 100;
}

void printArray() {
    cout<<"Array[";
    for(int i=0; i<SIZE-1; i++){
        cout<<arr[i] << ", ";
    }
    cout<<arr[SIZE-1] << "]"<<endl;
}

void printIsSorted(){
    if(isSorted){
        cout<<"The above array is sorted"<<endl;
    }
    else{
        cout<<"The above array is not sorted"<<endl;
    }
}


int main(){
    pthread_t workers[NUM_THREADS];
    pthread_attr_t attr[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    int chunkSize = SIZE/NUM_THREADS;
    
    fillUpArr(arr, SIZE);
    
    printArray();
    
    for (long i=0; i<NUM_THREADS; i++) {
        pthread_attr_init(&attr[i]);
        threadData[i].start = i*chunkSize;
        threadData[i].end = (i+1)*chunkSize;
        pthread_create(&workers[i], &attr[i], checkSort, (void*)&threadData[i]);
    }

    for (long i=0; i<NUM_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }
    printIsSorted();
    
    for (int i=0; i<SIZE; i++) {
        arr[i] = i;
    }

    isSorted = true;
    
    cout<< endl << endl << "************ The below is a run with a sorted array to showcase a positive scenario ************"<< endl << endl;
    
    printArray();
    for(long i=0; i<NUM_THREADS; i++) {
        pthread_attr_init(&attr[i]);
        threadData[i].start = i*chunkSize;
        threadData[i].end = (i+1)*chunkSize;
        pthread_create(&workers[i], &attr[i], checkSort, (void*)&threadData[i]);
    }

    for(long i=0; i<NUM_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }

    printIsSorted();
    
    pthread_mutex_destroy(&isSortedMutex);

    return 0;
}
