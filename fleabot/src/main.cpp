#include "TVMInference.h"
#include "KMeansClustering.h"
#include "UsbCamera.h"
#include "MLX90640Interface.h"

// MLX90640 irSensor(1, 0x33);
UsbCamera imageSensor(0, 640, 480, 30);
TVMCNNModel tvmModel;
KMeans kmeans("lib/centroids.bin", 3, 1536);
MLX90640 irSensor(1, 0x33);

int main() {
    // Initialize the TVM model
    tvmModel.load_model("lib/model_graph.json", "lib/model_lib.so", "lib/model_params.params");

    // Open the camera
    if (!imageSensor.openCamera()) {
        std::cerr << "Error opening camera" << std::endl;
        return 1;
    }

    // Start the MLX90640 sensor
    irSensor.start();

    // Main processing loop
    while (true) {

        // Get the latest frame from the IR sensor
        std::vector<float> frameData;
        if (!irSensor.getFrameData(frameData)) {
            std::cerr << "Error getting frame data" << std::endl;
            continue;
        }

        // Get the latest frame from the camera
        cv::Mat frame;
        if (!imageSensor.getLatestFrame(frame)) {
            std::cerr << "Error getting frame" << std::endl;
            continue;
        }

        // Convert the frame data to a 24x32 matrix
        std::vector<std::vector<float>> input_data(24, std::vector<float>(32));
        for (int i = 0; i < 24; ++i) {
            for (int j = 0; j < 32; ++j) {
                input_data[i][j] = frameData[i * 32 + j];
            }
        }

        // Set the input for the TVM model
        if (!tvmModel.set_input(input_data, 24, 32)) {
            std::cerr << "Error setting input" << std::endl;
            continue;
        }

        // Display the frame
        cv::imshow("Frame", frame);
        cv::waitKey(1);

        // Run the frame through the TVM model
        auto inference_result = tvmModel.run_inference();

        // Check if the inference was successful
        if (!inference_result) {
            std::cerr << "Error running inference" << std::endl;
            continue;
        }

        // Get the output of the inference
        auto output = tvmModel.get_output(0);

        // Perform KMeans clustering on the inference result
        auto cluster_result = kmeans.assignToCluster(output);
    }
    return 0;
}
