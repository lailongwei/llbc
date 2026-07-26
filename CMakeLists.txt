# CMake minimum version.
cmake_minimum_required (VERSION 3.16)

# Config solution.
project(llbc)
include("${CMAKE_SOURCE_DIR}/tools/cmake/config.cmake")

# Set project.
project(llbc VERSION ${LLBC_VERSION})

# Add llbc lib directory.
add_subdirectory(${LLBC_LIB_DIR})
# Add llbc lib tests directory.
add_subdirectory(${LLBC_LIB_TESTS_DIR})
# Add llbc wrap directory.
add_subdirectory(${LLBC_WRAP_DIR})
# Add tools directory.
add_subdirectory(${LLBC_TOOLS_DIR})

# Set VS solution startup project (Windows/Visual Studio generator only).
# NOTE: VS_STARTUP_PROJECT is a DIRECTORY property and must be set on the
#       top-level source dir (where project() lives), otherwise CMake ignores it.
if (WIN32 AND TARGET func_test_shared)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT func_test_shared)
endif()