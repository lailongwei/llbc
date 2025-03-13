# CMake minimum version.
cmake_minimum_required(VERSION 3.2)

# Set custom compiler, if you need.
# set(CUSTOM_COMPILE_TOOLSET_DIR "<custom compile toolset dir")
# set(STDC_LIB_DIR "${CUSTOM_COMPILE_TOOLSET_DIR}/lib64")
# set(CMAKE_CXX_COMPILER "${CUSTOM_COMPILE_TOOLSET_DIR}/bin/g++")
# set(CMAKE_C_COMPILER "${CUSTOM_COMPILE_TOOLSET_DIR}/bin/gcc")

# Set project name.
project(llbc)

# Set project c++ standard.
set(CMAKE_CXX_STANDARD 17)

# Set project cmake options.
set(CMAKE_COLOR_MAKEFILE on)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Disable min/max macro define(on MSVC compiler).
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

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