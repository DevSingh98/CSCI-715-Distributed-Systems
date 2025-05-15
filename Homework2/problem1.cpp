#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
using namespace std;

// Parallel prefix sum function to calculate the prefix sum of a given vector
void parallel_prefix_sum(vector<int>& arr, int start, int end) {
    // safe check incase the vector is a single element
    if (start == end) return;

    // Finding the halfway point
    int mid = (start + end) / 2;

    // Recursively compute prefix sums for the left half in a separate task
    // 'shared(arr)' means all tasks share access to the same array
    #pragma omp task shared(arr)
    parallel_prefix_sum(arr, start, mid);

    // Recursively compute prefix sums for the right half in another task
    #pragma omp task shared(arr)
    parallel_prefix_sum(arr, mid + 1, end);

    // Wait until both left and right recursive tasks finish
    #pragma omp taskwait

    // After the left half is processed, its total sum is arr[mid]
    // We add this offset to every element in the right half
    int offset = arr[mid];
    for (int i = mid + 1; i <= end; ++i) {
        arr[i] += offset;
    }
}

// function to do parallel prefix sum using openmp for
void parallel_prefix_sum_for(vector<int>& arr);

int main() {
    ifstream infile("prob1input.dat");
    ofstream outfile("prob1output.dat");
    vector<int> arr;
    int num;

    while (infile >> num) {
        arr.push_back(num);
    }

    #pragma omp parallel
    {
        #pragma omp single
        parallel_prefix_sum(arr, 0, arr.size() - 1);
    }
    
    // Lines 55 to 57 is only for the other parallel prefix sum using # for
//    if (!arr.empty()) {
//            parallel_prefix_sum_for(arr);
//        }

    for (int val : arr) {
        outfile << val << "\n";
    }

    return 0;
}


// Parallel prefix sum function using OpenMP for-loops (chunked approach)
void parallel_prefix_sum_for(vector<int>& arr) {
    int n = arr.size();
    int num_threads;
    vector<int> partial_sums;

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nthrds = omp_get_num_threads();

        // One thread initializes shared variables
        #pragma omp single
        {
            num_threads = nthrds;
            partial_sums.resize(num_threads, 0);
        }

        // create chunk size and start/end for each thread
        int chunk_size = (n + nthrds - 1) / nthrds;
        int start = tid * chunk_size;
        int end = min(start + chunk_size, n);

        // Each thread computes prefix sum in its chunk
        if (start < end) {
            for (int i = start + 1; i < end; ++i) {
                arr[i] += arr[i - 1];
            }
            // Save last element of chunk as partial sum
            partial_sums[tid] = arr[end - 1];
        }

        #pragma omp barrier

        // Thread 0 performs prefix sum on partial_sums
        #pragma omp single
        for (int i = 1; i < num_threads; ++i) {
            partial_sums[i] += partial_sums[i - 1];
        }

        #pragma omp barrier

        // Each thread (except thread 0) updates its chunk with offset
        if (tid > 0 && start < end) {
            int offset = partial_sums[tid - 1];
            for (int i = start; i < end; ++i) {
                arr[i] += offset;
            }
        }
    }
}
