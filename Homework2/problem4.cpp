#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <omp.h>

struct Point {
    double x, y;
    int cluster;
};

double distance(Point& a, Point& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

void kmeans(std::vector<Point>& points, int K, int max_iters=100) {
    int N = points.size();
    std::vector<Point> centroids(K);

    // Initialize centroids randomly
    for (int i = 0; i < K; ++i) {
        centroids[i] = points[rand() % N];
    }

    for (int iter = 0; iter < max_iters; ++iter) {
        // Assign points to nearest centroid
        #pragma omp parallel for
        for (int i = 0; i < N; ++i) {
            double min_dist = 1e9;
            int cluster = -1;
            for (int j = 0; j < K; ++j) {
                double d = distance(points[i], centroids[j]);
                if (d < min_dist) {
                    min_dist = d;
                    cluster = j;
                }
            }
            points[i].cluster = cluster;
        }

        // Recalculate centroids
        std::vector<double> sum_x(K, 0), sum_y(K, 0);
        std::vector<int> count(K, 0);

        for (const auto& p : points) {
            sum_x[p.cluster] += p.x;
            sum_y[p.cluster] += p.y;
            count[p.cluster]++;
        }

        for (int j = 0; j < K; ++j) {
            if (count[j] > 0) {
                centroids[j].x = sum_x[j] / count[j];
                centroids[j].y = sum_y[j] / count[j];
            }
        }
    }
}

int main() {
    std::ifstream infile("points_to_cluster.txt");
    std::vector<Point> points;
    double x, y;

    while (infile >> x >> y) {
        points.push_back({x, y, -1});
    }

    int K;
    std::cout << "Enter K: ";
    std::cin >> K;

    kmeans(points, K);

    std::ofstream outfile("cluster_output.txt");
    for (const auto& p : points) {
        outfile << p.x << "," << p.y << "," << p.cluster << "\n";
    }

    return 0;
}