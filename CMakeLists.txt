# CMake minimum version.
cmake_minimum_required (VERSION 3.16)

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

# ****************************************************************************
# Build options (mirror premake5_cfg.lua toggles).
option(LLBC_DISABLE_CXX11_ABI "Disable libstdc++ CXX11 ABI (define _GLIBCXX_USE_CXX11_ABI=0)" OFF)
option(LLBC_ENABLE_ASAN       "Enable AddressSanitizer (non-Windows only)"                    OFF)
option(LLBC_WARNINGS_AS_ERRORS "Treat warnings as errors (-Werror, non-MSVC)"                 ON)
option(LLBC_SYNC_VERSION      "Run llbc_prebuild.py to sync version into Version.cpp"         OFF)

# Default to Release when no build type / multi-config is specified.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set project c++ standard (strict C++17, no GNU extensions -> -std=c++17).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default hidden symbol visibility (mirror premake -fvisibility=hidden; works with LLBC_EXPORT/LLBC_HIDDEN).
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Debug/asan target file suffix (mirror premake targetsuffix: _debug / _asan / _asan_debug).
if (LLBC_ENABLE_ASAN)
	set(CMAKE_DEBUG_POSTFIX _asan_debug)
	set(CMAKE_RELEASE_POSTFIX _asan)
	set(CMAKE_RELWITHDEBINFO_POSTFIX _asan)
	set(CMAKE_MINSIZEREL_POSTFIX _asan)
else()
	set(CMAKE_DEBUG_POSTFIX _debug)
endif()

# Set project cmake options.
set(CMAKE_COLOR_MAKEFILE on)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Disable min/max macro define(on MSVC compiler).
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

# ****************************************************************************
# Common build flags carrier (mirror premake workspace-level c++ settings).
# Only llbc's own targets link this; 3rdparty(googletest) must NOT inherit -Werror etc.
add_library(llbc_build_flags INTERFACE)

if (NOT MSVC)
	target_compile_options(llbc_build_flags INTERFACE -Wall -Wno-strict-aliasing)
	if (LLBC_WARNINGS_AS_ERRORS)
		target_compile_options(llbc_build_flags INTERFACE -Werror)
	endif()
endif()

# DEBUG macro on debug builds (mirror premake "configurations:debug*" defines { "DEBUG" }).
target_compile_definitions(llbc_build_flags INTERFACE $<$<CONFIG:Debug>:DEBUG>)

# CXX11 ABI selection (non-Windows).
if (NOT WIN32)
	if (LLBC_DISABLE_CXX11_ABI)
		target_compile_definitions(llbc_build_flags INTERFACE _GLIBCXX_USE_CXX11_ABI=0)
	else()
		target_compile_definitions(llbc_build_flags INTERFACE _GLIBCXX_USE_CXX11_ABI=1)
	endif()
endif()

# Export dynamic symbol table for backtraces (mirror premake linkoptions { "-rdynamic" }).
if (NOT WIN32)
	target_link_options(llbc_build_flags INTERFACE -rdynamic)
endif()

# macOS: Foundation framework(.mm sources) + rpath so shared-linked exes find the dylib in the flat output dir.
if (APPLE)
	target_link_options(llbc_build_flags INTERFACE "SHELL:-framework Foundation")
	target_link_options(llbc_build_flags INTERFACE "SHELL:-Wl,-rpath,@loader_path")
endif()

# AddressSanitizer (non-Windows).
if (LLBC_ENABLE_ASAN AND NOT WIN32)
	target_compile_options(llbc_build_flags INTERFACE -fsanitize=address -g)
	target_link_options(llbc_build_flags INTERFACE -fsanitize=address)
endif()

# ****************************************************************************
# Set llbc core lib directory.
set(LLBC_LIB_DIR ${PROJECT_SOURCE_DIR}/llbc)
# Set llbc core lib tests directory.
set(LLBC_LIB_TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)
set(LLBC_LIB_EXAMPLE_DIR ${PROJECT_SOURCE_DIR}/tests/example)
set(LLBC_LIB_FUNC_TEST_DIR ${PROJECT_SOURCE_DIR}/tests/func_test)
set(LLBC_LIB_UNIT_TEST_DIR ${PROJECT_SOURCE_DIR}/tests/unit_test)
set(LLBC_LIB_QUICK_START_DIR ${PROJECT_SOURCE_DIR}/tests/quick_start)
# Set llbc wrap directory.
set(LLBC_WRAP_DIR ${PROJECT_SOURCE_DIR}/wrap)

# Set llbc output directory.
set(LLBC_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output/cmake)

# ****************************************************************************
# googletest (dependency of unit_test). Built from the git submodule via add_subdirectory,
# so it respects the same compiler/standard/ABI instead of the old fragile `cmake .. && make` custom target.
set(LLBC_GTEST_DIR ${LLBC_LIB_TESTS_DIR}/3rdparty/googletest)
if (EXISTS ${LLBC_GTEST_DIR}/CMakeLists.txt)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
	set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
	add_subdirectory(${LLBC_GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
	set(LLBC_HAS_GTEST ON)
else()
	message(WARNING "googletest submodule not found at ${LLBC_GTEST_DIR}; unit_test will be skipped. "
	                "Run: git submodule update --init tests/3rdparty/googletest")
	set(LLBC_HAS_GTEST OFF)
endif()

# Add sub directory.
add_subdirectory(${LLBC_LIB_DIR})
add_subdirectory(${LLBC_LIB_EXAMPLE_DIR})
add_subdirectory(${LLBC_LIB_FUNC_TEST_DIR})
if (LLBC_HAS_GTEST)
	add_subdirectory(${LLBC_LIB_UNIT_TEST_DIR})
endif()
add_subdirectory(${LLBC_LIB_QUICK_START_DIR})
