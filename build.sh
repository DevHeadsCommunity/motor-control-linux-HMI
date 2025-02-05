#!/bin/sh

# Create build directory if it doesn't exist
mkdir -p build

# Enter the build directory
cd build

# Run CMake with policy setting and suppress dev warnings
cmake ..

# Build using all available cores
make -j$(nproc || sysctl -n hw.ncpu)

./../bin/main
