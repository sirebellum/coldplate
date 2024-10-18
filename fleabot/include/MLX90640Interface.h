#ifndef MLX90640_H
#define MLX90640_H

#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

class MLX90640 {
public:
    MLX90640(int i2cBus = 1, uint8_t i2cAddress = 0x33);
    ~MLX90640();

    // Starts the data acquisition thread
    void start();

    // Stops the data acquisition thread
    void stop();

    // Gets the latest frame data (scaled between 0 and 1)
    bool getFrameData(std::vector<float>& frameData);

private:
    // I2C related
    int i2cBus;
    uint8_t i2cAddress;
    int i2cFileDescriptor;

    // Acquisition thread control
    std::thread acquisitionThread;
    std::atomic<bool> running;

    // Raw data storage
    std::vector<float> frameData;

    // Mutex for thread-safe access to frameData
    std::mutex dataMutex;

    // Initializes the I2C connection
    bool initialize();

    // Reads a full frame from the sensor
    bool readFrame();

    // Thread function for continuous data acquisition
    void acquisitionLoop();

    // Helper function to open the I2C device
    bool openI2C();

    // Helper function to close the I2C device
    void closeI2C();
};

#endif // MLX90640_H