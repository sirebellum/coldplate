#include "TVMInference.h"
#include "UsbCamera.h"
#include "MLX90640Interface.h"
#include "CatIdentifier.h"
#include "UART.h"

MLX90640 irSensor(1, 0x33);
UsbCamera imageSensor(0, 640, 480, 30);
TVMCNNModel tvmModel;
UARTComm uartComm("/dev/ttyUSB0", 115200);

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

    // Load and calculate centroids for the CatIdentifier
    cv::Mat zephyr_image = cv::imread("zephyr.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat flea_image = cv::imread("flea.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat zephyr_centroid, flea_centroid;

    // Do the tvm thing
    tvmModel.set_input(zephyr_image);
    tvmModel.run_inference();
    zephyr_centroid = tvmModel.get_output(0);

    tvmModel.set_input(flea_image);
    tvmModel.run_inference();
    flea_centroid = tvmModel.get_output(0);

    // Initialize the CatIdentifier
    CatIdentifier catIdentifier(zephyr_centroid, flea_centroid);

    // Main processing loop
    while (true) {

        // Get the latest frame from the IR sensor
        cv::Mat irFrame;
        if (!irSensor.getFrameData(irFrame)) {
            std::cerr << "Error getting frame from IR" << std::endl;
            continue;
        }

        // Get the latest frame from the camera
        cv::Mat frame;
        if (!imageSensor.getLatestFrame(frame)) {
            std::cerr << "Error getting frame from camera" << std::endl;
            continue;
        }

        // Set the input for the TVM model
        if (!tvmModel.set_input(irFrame)) {
            std::cerr << "Error setting input for TVM model" << std::endl;
            continue;
        }

        // Run the inference
        if (!tvmModel.run_inference()) {
            std::cerr << "Error running inference" << std::endl;
            continue;
        }

        // Get the output of the inference
        cv::Mat output = tvmModel.get_output(0);

        // Identify the cat
        std::string cat = catIdentifier.identifyCat(std::vector<double>(output.begin<float>(), output.end<float>()));

        // Send the result over UART
        std::vector<uint8_t> payload;
        if (cat == "Zephyr") {
            payload = {0x01};
        } else if (cat == "Flea") {
            payload = {0x02};
        } else {
            payload = {0x00};
        }

        if (!uartComm.sendCommand(payload)) {
            std::cerr << "Error sending command over UART" << std::endl;
        }
    }
    return 0;
}
