#include "UsbCamera.h"
#include <iostream>

// Constructor
UsbCamera::UsbCamera(int cameraIndex, int width, int height, int fps)
    : cameraIndex_(cameraIndex), width_(width), height_(height), fps_(fps), running_(false) {}

// Destructor
UsbCamera::~UsbCamera() {
    closeCamera();
}

// Opens the camera and starts the capture thread
bool UsbCamera::openCamera() {
    if (cap_.open(cameraIndex_)) {
        configureCamera();
        running_ = true;
        captureThread_ = std::thread(&UsbCamera::captureLoop, this);
        return true;
    }
    std::cerr << "Error: Unable to open camera at index " << cameraIndex_ << std::endl;
    return false;
}

// Closes the camera and stops the capture thread
void UsbCamera::closeCamera() {
    running_ = false;
    if (captureThread_.joinable()) {
        captureThread_.join();
    }
    if (cap_.isOpened()) {
        cap_.release();
    }
}

// Checks if the camera is open
bool UsbCamera::isCameraOpen() const {
    return cap_.isOpened();
}

// Retrieves the most recent frame
bool UsbCamera::getLatestFrame(cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(frameMutex_);
    if (latestFrame_.empty()) {
        return false;
    }
    latestFrame_.copyTo(frame);
    return true;
}

// Sets the resolution of the camera
void UsbCamera::setResolution(int width, int height) {
    width_ = width;
    height_ = height;
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_FRAME_WIDTH, width_);
        cap_.set(cv::CAP_PROP_FRAME_HEIGHT, height_);
    }
}

// Sets the frames per second
void UsbCamera::setFPS(int fps) {
    fps_ = fps;
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_FPS, fps_);
    }
}

// Sets the brightness level
void UsbCamera::setBrightness(double value) {
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_BRIGHTNESS, value);
    }
}

// Sets the contrast level
void UsbCamera::setContrast(double value) {
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_CONTRAST, value);
    }
}

// Sets the saturation level
void UsbCamera::setSaturation(double value) {
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_SATURATION, value);
    }
}

// Sets the exposure level
void UsbCamera::setExposure(double value) {
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_EXPOSURE, value);
    }
}

// Configures the camera with initial settings
void UsbCamera::configureCamera() {
    cap_.set(cv::CAP_PROP_FRAME_WIDTH, width_);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, height_);
    cap_.set(cv::CAP_PROP_FPS, fps_);
}

// The thread loop that captures frames continuously
void UsbCamera::captureLoop() {
    cv::Mat frame;
    while (running_) {
        if (cap_.read(frame)) {
            std::lock_guard<std::mutex> lock(frameMutex_);
            frame.copyTo(latestFrame_);
        } else {
            std::cerr << "Error: Failed to read frame from camera." << std::endl;
        }
    }
}
