#ifndef USBCAMERA_H
#define USBCAMERA_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <atomic>

class UsbCamera {
public:
    // Constructor & Destructor
    UsbCamera(int cameraIndex = 0, int width = 640, int height = 480, int fps = 30);
    ~UsbCamera();

    // Methods
    bool openCamera();
    void closeCamera();
    bool isCameraOpen() const;
    bool getLatestFrame(cv::Mat& frame);
    void setResolution(int width, int height);
    void setFPS(int fps);
    void setBrightness(double value);
    void setContrast(double value);
    void setSaturation(double value);
    void setExposure(double value);

private:
    int cameraIndex_;
    int width_;
    int height_;
    int fps_;
    cv::VideoCapture cap_;
    cv::Mat latestFrame_;
    std::thread captureThread_;
    std::mutex frameMutex_;
    std::atomic<bool> running_;

    // Internal helper methods
    void configureCamera();
    void captureLoop();
};

#endif // USBCAMERA_H
