#include "MLX90640Interface.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <chrono>

MLX90640::MLX90640(int i2cBus, uint8_t i2cAddress)
    : i2cBus(i2cBus), i2cAddress(i2cAddress), running(false), frameData(32 * 24, 0.0f) {}

MLX90640::~MLX90640() {
    stop();
    closeI2C();
}

bool MLX90640::initialize() {
    return openI2C();
}

void MLX90640::start() {
    if (!initialize()) {
        std::cerr << "Failed to initialize the MLX90640 sensor." << std::endl;
        return;
    }

    running = true;
    acquisitionThread = std::thread(&MLX90640::acquisitionLoop, this);
}

void MLX90640::stop() {
    running = false;
    if (acquisitionThread.joinable()) {
        acquisitionThread.join();
    }
}

bool MLX90640::getFrameData(std::vector<float>& frameData) {
    std::lock_guard<std::mutex> lock(dataMutex); // Ensure thread-safe access
    if (this->frameData.empty()) {
        return false;
    }
    memccpy(frameData.data(), this->frameData.data(), sizeof(float), this->frameData.size());
    return true;
}

void MLX90640::acquisitionLoop() {
    while (running) {
        if (!readFrame()) {
            std::cerr << "Failed to read frame from MLX90640." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust timing as necessary
    }
}

bool MLX90640::readFrame() {
    uint16_t rawData[32 * 24] = {0};

    // Read raw data from the sensor (mockup, you need to adjust based on MLX90640 protocol)
    if (read(i2cFileDescriptor, rawData, sizeof(rawData)) != sizeof(rawData)) {
        return false;
    }

    // Process and scale the raw data between 0 and 1
    {
        std::lock_guard<std::mutex> lock(dataMutex); // Ensure thread-safe access
        for (int i = 0; i < 32 * 24; ++i) {
            frameData[i] = static_cast<float>(rawData[i]) / 65535.0f; // Scale to [0, 1]
        }
    }

    return true;
}

bool MLX90640::openI2C() {
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", i2cBus);

    i2cFileDescriptor = open(filename, O_RDWR);
    if (i2cFileDescriptor < 0) {
        std::cerr << "Failed to open I2C bus " << i2cBus << std::endl;
        return false;
    }

    #define I2C_SLAVE 0x0703
    if (ioctl(i2cFileDescriptor, I2C_SLAVE, i2cAddress) < 0) {
        std::cerr << "Failed to acquire bus access and/or talk to slave." << std::endl;
        close(i2cFileDescriptor);
        return false;
    }

    return true;
}

void MLX90640::closeI2C() {
    if (i2cFileDescriptor >= 0) {
        close(i2cFileDescriptor);
    }
}