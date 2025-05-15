#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <omp.h>
using namespace std;

//structure to represent a 2d point
struct Point {
    double x, y;
    int cluster; //cluster assignment for each point
};

//function to compute euclidean distance between two points
double distance(Point& a, Point& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

//parallel k-means clustering algorithm
void kmeans(vector<Point>& points, int K, int max_iters=100) {
    int N = points.size();
    vector<Point> centroids(K);

    //initialize centroids randomly from existing points
    for (int i = 0; i < K; ++i) {
        centroids[i] = points[rand() % N];
    }

    for (int iter = 0; iter < max_iters; ++iter) {
        //assignment step: assign each point to the nearest centroid (parallelized)
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

        //update step: recompute centroids based on cluster assignments
        vector<double> sum_x(K, 0), sum_y(K, 0);
        vector<int> count(K, 0);

        //accumulate coordinate sums for each cluster
        for (const auto& p : points) {
            sum_x[p.cluster] += p.x;
            sum_y[p.cluster] += p.y;
            count[p.cluster]++;
        }

        //calculate the average to get new centroid positions
        for (int j = 0; j < K; ++j) {
            if (count[j] > 0) {
                centroids[j].x = sum_x[j] / count[j];
                centroids[j].y = sum_y[j] / count[j];
            }
        }
    }
}

int main() {
    ifstream infile("points_to_cluster.txt");
    vector<Point> points;
    double x, y;

    //read input points from file
    while (infile >> x >> y) {
        points.push_back({x, y, -1}); //initialize with unassigned cluster
    }

    int K;
    cout << "Enter K: ";
    cin >> K;

    //run k-means algorithm
    kmeans(points, K);

    //write final clusters to output file
    ofstream outfile("cluster_output.txt");
    for (const auto& p : points) {
        outfile << p.x << "," << p.y << "," << p.cluster << "\n";
    }

    return 0;
}