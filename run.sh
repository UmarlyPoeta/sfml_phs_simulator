#!/bin/bash

# Set the build directory and executable name
BUILD_DIR="build"
EXECUTABLE="sfml_phs_simulator"

# Check if the executable exists
if [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
    echo "Executable not found. Building the project..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake ..
    make
    cd ..
fi

# Run the SFML project
"./$BUILD_DIR/$EXECUTABLE"