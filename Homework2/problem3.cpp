#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
using namespace std;

bool is_sorted_parallel(const vector<int>& arr, int start, int end) {
    if (start >= end) return true;

    int mid = (start + end) / 2;
    bool left_sorted = true, right_sorted = true;

    #pragma omp task shared(left_sorted)
    left_sorted = is_sorted_parallel(arr, start, mid);

    #pragma omp task shared(right_sorted)
    right_sorted = is_sorted_parallel(arr, mid+1, end);

    #pragma omp taskwait

    if (arr[mid] > arr[mid+1]) return false;

    return left_sorted && right_sorted;
}

int main() {
    ifstream infile("prob3input.dat");
    vector<int> arr;
    int num;

    while (infile >> num) {
        arr.push_back(num);
    }

    bool sorted;
    #pragma omp parallel
    {
        #pragma omp single
        sorted = is_sorted_parallel(arr, 0, arr.size()-1);
    }

    if (sorted) {
        cout << "Sorted\n";
    } else {
        cout << "Not Sorted\n";
    }

    return 0;
}