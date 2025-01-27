#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! You can run the program with:"
    echo "./DepreVisuales"
else
    echo "Build failed. Please check the error messages above."
fi 