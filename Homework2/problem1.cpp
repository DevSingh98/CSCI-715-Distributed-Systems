#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

void parallel_prefix_sum(std::vector<int>& arr, int start, int end) {
    if (start == end) return;

    int mid = (start + end) / 2;

    #pragma omp task shared(arr)
    parallel_prefix_sum(arr, start, mid);

    #pragma omp task shared(arr)
    parallel_prefix_sum(arr, mid + 1, end);

    #pragma omp taskwait

    int offset = arr[mid];
    for (int i = mid + 1; i <= end; ++i) {
        arr[i] += offset;
    }
}

int main() {
    std::ifstream infile("prob1input.dat");
    std::ofstream outfile("prob1output.dat");
    std::vector<int> arr;
    int num;

    while (infile >> num) {
        arr.push_back(num);
    }

    #pragma omp parallel
    {
        #pragma omp single
        parallel_prefix_sum(arr, 0, arr.size() - 1);
    }

    for (int val : arr) {
        outfile << val << "\n";
    }

    return 0;
}
