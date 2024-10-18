#include "CatIdentifier.h"
#include <cmath>
#include <opencv2/opencv.hpp>

CatIdentifier::CatIdentifier(const std::vector<double>& initial_zephyr, const std::vector<double>& initial_flea)
    : zephyr_centroid(cv::Mat(initial_zephyr).reshape(1, 1)), 
      flea_centroid(cv::Mat(initial_flea).reshape(1, 1)), 
      threshold(0.1), learningRate(0.05) {}

// Identify the cat based on the new set of distances
std::string CatIdentifier::identifyCat(const std::vector<double>& distances) {
    cv::Mat newPoint = cv::Mat(distances).reshape(1, 1);
    double distToZephyr = computeDistance(newPoint, zephyr_centroid);
    double distToFlea = computeDistance(newPoint, flea_centroid);

    if (distToZephyr < threshold) {
        updateCentroid(zephyr_centroid, newPoint);
        return "Zephyr";
    } else if (distToFlea < threshold) {
        updateCentroid(flea_centroid, newPoint);
        return "Flea";
    }
    return "Unknown";
}

// Set a new threshold for classification
void CatIdentifier::setThreshold(double newThreshold) {
    threshold = newThreshold;
}

// Set a new learning rate for centroid adjustment
void CatIdentifier::setLearningRate(double newLearningRate) {
    learningRate = newLearningRate;
}

// Manually update the Zephyr centroid based on new labeled data
void CatIdentifier::manualUpdateZephyr(const std::vector<double>& newData) {
    cv::Mat newPoint = cv::Mat(newData).reshape(1, 1);
    updateCentroid(zephyr_centroid, newPoint);
}

// Manually update the Flea centroid based on new labeled data
void CatIdentifier::manualUpdateFlea(const std::vector<double>& newData) {
    cv::Mat newPoint = cv::Mat(newData).reshape(1, 1);
    updateCentroid(flea_centroid, newPoint);
}

// Get the current Zephyr centroid
std::vector<double> CatIdentifier::getZephyrCentroid() const {
    return std::vector<double>(zephyr_centroid.begin<double>(), zephyr_centroid.end<double>());
}

// Get the current Flea centroid
std::vector<double> CatIdentifier::getFleaCentroid() const {
    return std::vector<double>(flea_centroid.begin<double>(), flea_centroid.end<double>());
}

// Compute the Euclidean distance between two vectors
double CatIdentifier::computeDistance(const cv::Mat& a, const cv::Mat& b) const {
    return cv::norm(a - b);
}

// Update a centroid using a simple moving average approach
void CatIdentifier::updateCentroid(cv::Mat& centroid, const cv::Mat& newPoint) {
    centroid += learningRate * (newPoint - centroid);
}
