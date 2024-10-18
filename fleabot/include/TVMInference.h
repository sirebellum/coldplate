#ifndef TVM_CNN_INTERFACE_H
#define TVM_CNN_INTERFACE_H

#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/ndarray.h>
#include <vector>
#include <string>

class TVMCNNModel {
public:
    /**
     * @brief Constructor for TVMCNNModel class.
     */
    TVMCNNModel();

    /**
     * @brief Destructor for TVMCNNModel class.
     */
    ~TVMCNNModel();

    /**
     * @brief Load the compiled TVM model files.
     * 
     * @param graph_path Path to the TVM graph file (usually .json).
     * @param lib_path Path to the TVM shared library (usually .so).
     * @param params_path Path to the TVM parameters file (usually .params).
     * @return True if the model was loaded successfully, false otherwise.
     */
    bool load_model(const std::string& graph_path, const std::string& lib_path, const std::string& params_path);

    /**
     * @brief Set the input for the model.
     * 
     * @param input_data 2D float array containing the input data.
     * @param height The height of the 2D array.
     * @param width The width of the 2D array.
     * @return True if the input was successfully set, false otherwise.
     */
    bool set_input(const std::vector<std::vector<float>>& input_data, int height, int width);

    /**
     * @brief Run the inference using the loaded model.
     * 
     * @return True if the inference was successful, false otherwise.
     */
    bool run_inference();

    /**
     * @brief Get the output of the inference.
     * 
     * @return A vector of floats containing the output.
     */
    std::vector<float> get_output(int output_index);

private:
    tvm::runtime::PackedFunc set_input_func;
    tvm::runtime::PackedFunc run_func;
    tvm::runtime::PackedFunc get_output_func;
    tvm::runtime::NDArray input_tensor;
    tvm::runtime::NDArray output_tensor;
    DLDevice dev = {kDLCPU, 0};;

    int input_height;
    int input_width;

    /**
     * @brief Internal function to handle memory allocation and preparation for inputs.
     */
    void allocate_input_tensor(int height, int width);

    /**
     * @brief Helper to load parameters from the file.
     */
    bool load_params(const std::string& params_path);
};

#endif // TVM_CNN_INTERFACE_H
