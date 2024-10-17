cmake_minimum_required(VERSION 3.10)
project(fleabot)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(third_party)

# Link libraries
find_package(TVM REQUIRED)
find_package(KMeansClustering REQUIRED)
find_package(SoundLibrary REQUIRED)

# Add executable
add_executable(main src/main.cpp)

# Link necessary libraries
target_link_libraries(main PRIVATE TVM::tvm KMeansClustering::kmeans SoundLibrary::soundlib)

# I2C linking may require additional configuration depending on your platform
find_library(I2C_LIB NAMES i2c)
target_link_libraries(main PRIVATE ${I2C_LIB})

