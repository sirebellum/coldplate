#include "TVMInference.h"
#include <fstream>
#include <iostream>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/serializer.h>
#include <tvm/runtime/device_api.h>
#include <opencv2/opencv.hpp>

TVMCNNModel::TVMCNNModel() : input_height(0), input_width(0) {}

TVMCNNModel::~TVMCNNModel() {}

bool TVMCNNModel::load_model(const std::string& graph_path, const std::string& lib_path, const std::string& params_path) {
    // Load the compiled shared library
    tvm::runtime::Module mod_syslib = tvm::runtime::Module::LoadFromFile(lib_path);

    // Load graph JSON
    std::ifstream json_in(graph_path);
    std::stringstream json_ss;
    json_ss << json_in.rdbuf();
    std::string json_data = json_ss.str();

    // Load parameters
    std::ifstream params_in(params_path, std::ios::binary);
    std::stringstream params_ss;
    params_ss << params_in.rdbuf();
    std::string params_data = params_ss.str();

    // Create graph runtime
    tvm::runtime::Module graph_runtime = (*tvm::runtime::Registry::Get("tvm.graph_executor.create"))
        (json_data, mod_syslib, static_cast<int>(dev.device_type), dev.device_id);

    // Load parameters into graph runtime
    tvm::runtime::PackedFunc load_params = graph_runtime.GetFunction("load_params");
    load_params(tvm::runtime::String(params_data));

    // Get the set_input, run, and get_output functions
    set_input_func = graph_runtime.GetFunction("set_input");
    run_func = graph_runtime.GetFunction("run");
    get_output_func = graph_runtime.GetFunction("get_output");

    return true;
}

bool TVMCNNModel::set_input(const cv::Mat& input_image) {
    // Ensure the input image is of the expected type (e.g., CV_32FC1)
    if (input_image.empty() || input_image.type() != CV_32FC1) {
        std::cerr << "Invalid input image. Expected non-empty CV_32FC1 matrix." << std::endl;
        return false;
    }

    input_height = input_image.rows;
    input_width = input_image.cols;
    allocate_input_tensor(input_height, input_width);

    // Copy data into the input tensor
    auto* data_ptr = static_cast<float*>(input_tensor->data);
    for (int i = 0; i < input_height; ++i) {
        for (int j = 0; j < input_width; ++j) {
            data_ptr[i * input_width + j] = input_image.at<float>(i, j);
        }
    }

    // Set the input to the model
    try {
        set_input_func("input", input_tensor);
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to set input: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool TVMCNNModel::run_inference() {
    try {
        run_func();
    } catch (const std::runtime_error& e) {
        std::cerr << "Inference failed: " << e.what() << std::endl;
        return false;
    }

    return true;
}

cv::Mat TVMCNNModel::get_output(int output_index) {
    try {
        tvm::runtime::NDArray output = get_output_func(output_index);
        int64_t size = output.Shape()[0];

        // Create a cv::Mat to hold the output, assuming a 1D output
        cv::Mat result(1, size, CV_32F);
        auto* output_data = static_cast<float*>(output->data);

        for (int64_t i = 0; i < size; ++i) {
            result.at<float>(0, i) = output_data[i];
        }

        return result;
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to retrieve output: " << e.what() << std::endl;
        return cv::Mat();
    }
}

void TVMCNNModel::allocate_input_tensor(int height, int width) {
    // Define shape for input tensor
    std::vector<int64_t> shape = {1, 1, height, width};
    input_tensor = tvm::runtime::NDArray::Empty(shape, tvm::DataType::Float(32), this->dev);
}
