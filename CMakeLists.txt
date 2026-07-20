# CMake minimum version.
cmake_minimum_required (VERSION 3.16)

# Config solution.
include("${CMAKE_SOURCE_DIR}/tools/cmake/config.cmake")

# Set project.
project(llbc VERSION "${LLBC_VERSION}")

# Note: coverage instrumentation (LLBC_ENABLE_COVERAGE) is applied in tools/cmake/config.cmake
# via the llbc_sln_build_settings interface library.

# Add llbc lib directory.
add_subdirectory(${LLBC_LIB_DIR})
# Add llbc lib test directories.
add_subdirectory(${LLBC_LIB_EXAMPLE_DIR})
add_subdirectory(${LLBC_LIB_FUNC_TEST_DIR})
if (EXISTS "${LLBC_3RD_DIR_GOOGLETEST}/CMakeLists.txt")
	# Build googletest from the submodule -> provides the `gtest` target unit_test links against.
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
	set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
	add_subdirectory(${LLBC_3RD_DIR_GOOGLETEST} ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
	add_subdirectory(${LLBC_LIB_UNIT_TEST_DIR})
else()
	message(NOTICE "3rd - googletest not found(dir: ${LLBC_3RD_DIR_GOOGLETEST}), unit_test will be skipped")
	message(NOTICE "- run <git submodule update --init tests/3rdparty/googletest> to update")
endif()
add_subdirectory(${LLBC_LIB_QUICK_START_DIR})
