#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    int N;
    cout << "Enter N: ";
    cin >> N;

    // vector to hold whether that number is prime, initalizing both 0 and 1 as false and the rest true
    vector<bool> is_prime(N + 1, true);
    is_prime[0] = is_prime[1] = false;

    // We only need to check for factors up to sqrt(N)
    int limit = sqrt(N);

    // Loop through all potential prime candidates up to sqrt(N)
    // This part is sequential to ensure correctness of the primality check (tried to parallelize this but caused non prime numbers to be left as prime)
    for (int p = 2; p <= limit; ++p) {
        // If p is still marked as prime
        if (is_prime[p]) {
            // Mark all multiples of p (starting from p*p) as not prime
            // This inner loop is safe to parallelize because every thread writes false
            #pragma omp parallel for schedule(dynamic)
            for (int i = p * p; i <= N; i += p) {
                is_prime[i] = false;
            }
        }
    }

    // Write all numbers marked as prime to the output file
    ofstream outfile("prob2output.dat");
    for (int i = 2; i <= N; ++i) {
        if (is_prime[i]) {
            outfile << i << "\n";
        }
    }

    return 0;
}
