# CMake minimum version.
cmake_minimum_required(VERSION 2.8)

# Set project name.
project(llbc)

# Set project c++ standard.
set(CMAKE_CXX_STANDARD 14)

# Set project cmake options.
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_COLOR_MAKEFILE on)

# Set llbc core lib directory.
set(LLBC_LIB_DIR ${PROJECT_SOURCE_DIR}/llbc)

# Set llbc testsuite directory.
set(LLBC_TESTSUITE_DIR ${PROJECT_SOURCE_DIR}/testsuite)

# Set llbc wrap directory.
set(LLBC_WRAP_DIR ${PROJECT_SOURCE_DIR}/wrap)

# Set llbc output directory.
set(LLBC_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output/cmake)

# Add sub directory.
add_subdirectory(${LLBC_LIB_DIR})
add_subdirectory(${LLBC_TESTSUITE_DIR})