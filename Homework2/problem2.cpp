#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <omp.h>

int main() {
    int N;
    std::cout << "Enter N: ";
    std::cin >> N;

    std::vector<bool> is_prime(N+1, true);
    is_prime[0] = is_prime[1] = false;

    int limit = std::sqrt(N);

    #pragma omp parallel for schedule(dynamic)
    for (int p = 2; p <= limit; ++p) {
        if (is_prime[p]) {
            for (int i = p * p; i <= N; i += p) {
                is_prime[i] = false;
            }
        }
    }

    std::ofstream outfile("prob2output.dat");
    for (int i = 2; i <= N; ++i) {
        if (is_prime[i]) outfile << i << "\n";
    }

    return 0;
}