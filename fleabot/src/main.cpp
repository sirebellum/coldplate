#include <thread>
#include <atomic>
#include "MLX90640Interface.h"
#include "TVMInference.h"
#include "KMeansClustering.h"
#include "Speaker.h"

// Global flag to indicate when a new frame is available
std::atomic<bool> new_frame_available(false);
MLX90640Interface irSensor;
TVMInference tvmModel;
KMeansClustering kmeans;
Speaker speaker;

// Function that runs in a separate thread to acquire IR data
void acquire_data() {
    while (true) {
        // Acquire the latest IR frame
        irSensor.acquireFrame();
        
        // Notify the main loop that a new frame is available
        new_frame_available.store(true);
        
        // Simulate a delay to avoid rapid polling
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    // Start the data acquisition in a separate thread
    std::thread acquisition_thread(acquire_data);

    // Main processing loop
    while (true) {
        // Wait until a new frame is available
        if (new_frame_available.load()) {
            new_frame_available.store(false);  // Reset the flag

            // Get the most recent frame from the IR sensor
            auto ir_frame = irSensor.getFrameData();

            // Run the frame through the TVM model
            auto inference_result = tvmModel.runInference(ir_frame);

            // Perform KMeans clustering on the inference result
            auto cluster_result = kmeans.runClustering(inference_result);

            // Based on the cluster result, play a sound
            speaker.playSound(cluster_result);
        }

        // Simulate a short delay to avoid excessive CPU usage in the main loop
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Join the acquisition thread (though this may never be reached)
    acquisition_thread.join();

    return 0;
}
