# CMake minimum version.
cmake_minimum_required (VERSION 3.2...3.5)

# Setup c/cxx compilers and linker, if needed.
# set(CUSTOM_COMPILE_TOOLSET_DIR "<custom compile toolset dir>")
if (CUSTOM_COMPILE_TOOLSET_DIR)
	message("Use custom compile toolset, dir:${CUSTOM_COMPILE_TOOLSET_DIR}")
	set(STDC_LIB_DIR "${CUSTOM_COMPILE_TOOLSET_DIR}/lib64")
	set(CMAKE_C_COMPILER "${CUSTOM_COMPILE_TOOLSET_DIR}/bin/gcc")
	set(CMAKE_CXX_COMPILER "${CUSTOM_COMPILE_TOOLSET_DIR}/bin/g++")
	set(CMAKE_LINKER "${CUSTOM_COMPILE_TOOLSET_DIR}/bin/ld")
endif()

# Output compilers and linker.
message("Compiler/Linker:")
message("==> c compiler: ${CMAKE_C_COMPILER}")
message("==> c++ compiler: ${CMAKE_CXX_COMPILER}")
message("==> linker: ${CMAKE_LINKER}")
message("==> stdc lib dir: ${STDC_LIB_DIR}")

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
# Set llbc core lib tests directory.
set(LLBC_LIB_EXAMPLE_DIR ${PROJECT_SOURCE_DIR}/tests/example)
set(LLBC_LIB_FUNC_TEST_DIR ${PROJECT_SOURCE_DIR}/tests/func_test)
set(LLBC_LIB_UNIT_TEST_DIR ${PROJECT_SOURCE_DIR}/tests/unit_test)
set(LLBC_LIB_QUICK_START_DIR ${PROJECT_SOURCE_DIR}/tests/quick_start)
# Set llbc wrap directory.
set(LLBC_WRAP_DIR ${PROJECT_SOURCE_DIR}/wrap)

# Set llbc output directory.
set(LLBC_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output/cmake)

# Add sub directory.
add_subdirectory(${LLBC_LIB_DIR})
add_subdirectory(${LLBC_LIB_EXAMPLE_DIR})
add_subdirectory(${LLBC_LIB_FUNC_TEST_DIR})
add_subdirectory(${LLBC_LIB_UNIT_TEST_DIR})
add_subdirectory(${LLBC_LIB_QUICK_START_DIR})
