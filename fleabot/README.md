# fleabot

This project performs image processing using TVM for inference, I2C for sensor interfacing, KMeans clustering, and sound generation.

## Project Structure

- **src/**: Source code
- **include/**: Header files
- **lib/**: Custom and third-party libraries
- **third_party/**: External dependencies (TVM, KMeans, etc.)
- **cmake/**: CMake-related scripts and configuration
- **scripts/**: Helper scripts

## Modules

- **TVMInference**: Handles inference using TVM.
- **MLX90640Interface**: Manages interfacing with the MLX90640 IR sensor over I2C.
- **Speaker**: Generates sound using a connected speaker.
- **KMeansClustering**: Runs clustering algorithms on the TVM model output.
