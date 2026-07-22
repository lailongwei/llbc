# CMake minimum version.
cmake_minimum_required (VERSION 3.16)

# Config solution.
include("${CMAKE_SOURCE_DIR}/tools/cmake/config.cmake")

# Set project.
project(llbc VERSION "${LLBC_VERSION}")

# Add llbc lib directory.
add_subdirectory(${LLBC_LIB_DIR})
# Add llbc lib tests directory.
add_subdirectory(${LLBC_LIB_TESTS_DIR})
