# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LumosReferenceTesting is a C++ project using CMake for build configuration. The project currently contains a simple application that demonstrates basic mathematical computations with vectors.

## Build System

This project uses CMake with the following configuration:
- C++17 standard
- Debug build type by default
- Includes Google Test for testing framework
- Includes nlohmann/json library for JSON processing

### Building the Project

```bash
# Create build directory and configure
mkdir -p build && cd build
cmake ..

# Build the project
make

# Run the simple application
./src/applications/simple/simple
```

### Running Tests

```bash
# From build directory
ctest
# Or run tests with verbose output
ctest -V
```

## Architecture

The project follows a standard C++ project structure:

- `src/applications/simple/` - Contains the main application with a simple mathematical computation example
- `third_party/` - External dependencies (Google Test and nlohmann/json)
- `CMakeLists.txt` - Root CMake configuration
- `build/` - Build artifacts (generated)

The main application (`src/applications/simple/main.cpp`) demonstrates:
- Vector operations with mathematical functions
- Time series data generation using sine waves
- Basic C++ standard library usage

## Third-Party Dependencies

- **Google Test**: Testing framework located in `third_party/googletest/`
- **nlohmann/json**: JSON library located in `third_party/nlohmann/` (currently commented out in main.cpp)

## Development Workflow

1. Make changes to source files in `src/`
2. Build from the `build/` directory using `make`
3. Run tests using `ctest`
4. Execute applications from the build directory