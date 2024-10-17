#include "KMeans.h"
#include <fstream>
#include <cmath>
#include <thread>
#include <limits>
#include <iostream>

// Constructor to initialize centroids and vector size
KMeans::KMeans(const std::string& centroidsFile, int num_centroids, int vector_size)
    : num_centroids(num_centroids), vector_size(vector_size) {
    loadCentroids(centroidsFile);
}

// Helper function to load centroids from a binary file
void KMeans::loadCentroids(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open centroid file");
    }

    centroids.resize(num_centroids, std::vector<float>(vector_size));
    for (int i = 0; i < num_centroids; ++i) {
        file.read(reinterpret_cast<char*>(centroids[i].data()), vector_size * sizeof(float));
    }

    file.close();
}

// Function to compute the Euclidean distance between two vectors
float KMeans::computeDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    for (int i = 0; i < vector_size; ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

// Function to assign a single data point to the nearest centroid
int KMeans::assignToCluster(const std::vector<float>& data_point) {
    float min_distance = std::numeric_limits<float>::max();
    int best_cluster = -1;

    for (int i = 0; i < num_centroids; ++i) {
        float distance = computeDistance(data_point, centroids[i]);
        if (distance < min_distance) {
            min_distance = distance;
            best_cluster = i;
        }
    }

    return best_cluster;
}

// Multithreaded function to assign multiple data points
void KMeans::assignClusters(const std::vector<std::vector<float>>& data, std::vector<int>& cluster_assignments) {
    int num_threads = std::thread::hardware_concurrency();
    int chunk_size = data.size() / num_threads;
    std::vector<std::thread> threads;

    auto worker = [&](int start, int end) {
        for (int i = start; i < end; ++i) {
            int cluster = assignToCluster(data[i]);
            {
                std::lock_guard<std::mutex> lock(mtx);
                cluster_assignments[i] = cluster;
            }
        }
    };

    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = (i == num_threads - 1) ? data.size() : start + chunk_size;
        threads.emplace_back(worker, start, end);
    }

    for (auto& t : threads) {
        t.join();
    }
}
