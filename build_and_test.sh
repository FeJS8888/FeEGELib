#!/bin/bash
# FeEGELib Test Build and Run Script
# Note: This library requires Windows environment to run
# This script is for documentation purposes

echo "========================================"
echo "FeEGELib Test Suite Build Script"
echo "========================================"
echo ""
echo "⚠️  WARNING: This library requires Windows and GDI/GDI+ libraries"
echo "   This script is for reference only on non-Windows systems"
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure CMake
echo ""
echo "Configuring CMake..."
cmake .. || {
    echo "❌ CMake configuration failed!"
    echo "   Make sure you're on Windows with proper toolchain"
    exit 1
}

# Build
echo ""
echo "Building test programs..."
cmake --build . || {
    echo "❌ Build failed!"
    exit 1
}

echo ""
echo "========================================"
echo "✓ Build successful!"
echo "Test executables are in build/bin/"
echo "========================================"
echo ""

# Ask to run tests
read -p "Run all tests now? (y/n): " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Running tests..."
    echo "========================================"
    
    echo ""
    echo "[1/4] Running Base module tests..."
    echo "----------------------------------------"
    ./bin/test_base || echo "⚠️  test_base failed"
    
    echo ""
    echo "[2/4] Running Collision module tests..."
    echo "----------------------------------------"
    ./bin/test_collision || echo "⚠️  test_collision failed"
    
    echo ""
    echo "[3/4] Running Element basic tests..."
    echo "----------------------------------------"
    ./bin/test_element_basic || echo "⚠️  test_element_basic failed"
    
    echo ""
    echo "[4/4] Running Layout module tests..."
    echo "----------------------------------------"
    ./bin/test_layout || echo "⚠️  test_layout failed"
    
    echo ""
    echo "========================================"
    echo "All tests completed!"
    echo "========================================"
fi

cd ..
