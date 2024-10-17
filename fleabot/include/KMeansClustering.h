#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include <string>
#include <mutex>

class KMeans {
private:
    std::vector<std::vector<float>> centroids;  // Precomputed centroids
    int num_centroids;                          // Number of centroids
    int vector_size;                            // Dimensionality of the input data
    std::mutex mtx;

    // Helper function to load centroids from a binary file
    void loadCentroids(const std::string& filePath);

    // Function to compute the Euclidean distance between two vectors
    float computeDistance(const std::vector<float>& a, const std::vector<float>& b);

public:
    // Constructor
    KMeans(const std::string& centroidsFile, int num_centroids = 3, int vector_size = 128);

    // Function to assign a single data point to the nearest centroid
    int assignToCluster(const std::vector<float>& data_point);

    // Multithreaded function to assign multiple data points
    void assignClusters(const std::vector<std::vector<float>>& data, std::vector<int>& cluster_assignments);
};

#endif // KMEANS_H
