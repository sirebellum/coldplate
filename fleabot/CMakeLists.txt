cmake_minimum_required(VERSION 3.10)
project(fleabot)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add include directories
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Add subdirectory files
file(GLOB_RECURSE SUB_SOURCES "src/*/*.cpp")

# Copy lib folder
file(COPY lib DESTINATION ${CMAKE_BINARY_DIR})

# Add executable
add_executable(main ${SOURCES} ${SUB_SOURCES} ${LIB_SOURCES})

# Link libraries
find_package(TVM REQUIRED)
find_package(OpenCV REQUIRED)

# Link necessary libraries
target_link_libraries(main PRIVATE tvm::tvm_runtime ${OpenCV_LIBS})

# Include TVM headers
target_include_directories(main PRIVATE ${TVM_INCLUDE_DIRS})
