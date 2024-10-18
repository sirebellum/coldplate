#ifndef CATIDENTIFIER_H
#define CATIDENTIFIER_H

#include <string>
#include <opencv2/opencv.hpp>

class CatIdentifier {
public:
    CatIdentifier(const std::vector<double>& initial_zephyr, const std::vector<double>& initial_flea);

    // Method to classify a new set of distances
    std::string identifyCat(const std::vector<double>& distances);
    
    // Setters for adjusting parameters
    void setThreshold(double newThreshold);
    void setLearningRate(double newLearningRate);
    
    // Manually update the centroids if needed
    void manualUpdateZephyr(const std::vector<double>& newData);
    void manualUpdateFlea(const std::vector<double>& newData);
    
    // Get the current centroids (for monitoring)
    std::vector<double> getZephyrCentroid() const;
    std::vector<double> getFleaCentroid() const;
    
private:
    cv::Mat zephyr_centroid;
    cv::Mat flea_centroid;
    double threshold;
    double learningRate;
    
    // Helper methods
    double computeDistance(const cv::Mat& a, const cv::Mat& b) const;
    void updateCentroid(cv::Mat& centroid, const cv::Mat& newPoint);
};

#endif // CATIDENTIFIER_H
