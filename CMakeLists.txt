# CMake minimum version.
cmake_minimum_required(VERSION 3.2)

# Set project name.
project(llbc)

# Set project c++ standard.
set(CMAKE_CXX_STANDARD 14)

# Set project cmake options.
SET(CMAKE_VERBOSE_MAKEFILE on)
SET(CMAKE_COLOR_MAKEFILE on)

# Set llbc core lib directory.
SET(LLBC_CORE_LIB_DIR ${PROJECT_SOURCE_DIR}/llbc)

# Set llbc testsuite directory.
SET(LLBC_TEST_DIR ${PROJECT_SOURCE_DIR}/testsuite)

# Set llbc wrap directory.
SET(LLBC_WRAP_DIR ${PROJECT_SOURCE_DIR}/wrap)

# Set llbc output directory.
SET(LLBC_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output/cmake)

# Add sub directory.
add_subdirectory(${LLBC_CORE_LIB_DIR})
add_subdirectory(${LLBC_TEST_DIR})
#add_subdirectory(${LLBC_WRAP_DIR})