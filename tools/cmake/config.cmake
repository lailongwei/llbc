# ==================== User mutable settings ====================
# CXX11 abi disable switch.
option(LLBC_DISABLE_CXX11_ABI  "Disable libstdc++ CXX11 ABI (define _GLIBCXX_USE_CXX11_ABI=0)" OFF)
# Asan support switch.
option(LLBC_ENABLE_ASAN "Enable AddressSanitizer (non-Windows only)" OFF)
# Coverage support switch.
option(LLBC_ENABLE_COVERAGE "Enable coverage (non-Windows only)" OFF)
# Custom c/cpp compile toolset dir setting.
# set(LLBC_CUSTOM_COMPILE_TOOLSET_DIR "<custom compile toolset dir>" CACHE STRING "Custom compile toolset dir")

# ==================== User imutable settings ====================
# Some directories define.
# - llbc top directory.
set(LLBC_TOP_DIR "${CMAKE_SOURCE_DIR}")
# - llbc core library directory.
set(LLBC_LIB_DIR "${LLBC_TOP_DIR}/llbc")
# - llbc core library tests directory.
set(LLBC_LIB_TESTS_DIR "${LLBC_TOP_DIR}/tests")
set(LLBC_LIB_EXAMPLE_DIR ${LLBC_LIB_TESTS_DIR}/example)
set(LLBC_LIB_FUNC_TEST_DIR ${LLBC_LIB_TESTS_DIR}/func_test)
set(LLBC_LIB_UNIT_TEST_DIR ${LLBC_LIB_TESTS_DIR}/unit_test)
set(LLBC_LIB_QUICK_START_DIR ${LLBC_LIB_TESTS_DIR}/quick_start)
# - Set llbc wrap libraries directory.
set(LLBC_WRAP_DIR ${LLBC_TOP_DIR}/wrap)
# - 3rd direstories.
set(LLBC_3RD_DIR_GOOGLETEST "${LLBC_LIB_TESTS_DIR}/3rdparty/googletest")
# - Set llbc output directory.
set(LLBC_OUTPUT_DIR ${LLBC_TOP_DIR}/output/cmake)
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

# Set project c++ standard (strict C++17, no GNU extensions -> -std=c++17).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default hidden symbol visibility.
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Set c/cxx compilers and linker, if needed.
if (LLBC_CUSTOM_COMPILE_TOOLSET_DIR)
	message(STATUS "Use custom c/cpp compile toolset, dir:${LLBC_CUSTOM_COMPILE_TOOLSET_DIR}")
	set(CMAKE_C_COMPILER "${LLBC_CUSTOM_COMPILE_TOOLSET_DIR}/bin/gcc")
	set(CMAKE_CXX_COMPILER "${LLBC_CUSTOM_COMPILE_TOOLSET_DIR}/bin/g++")
	set(CMAKE_LINKER "${LLBC_CUSTOM_COMPILE_TOOLSET_DIR}/bin/ld")
endif()

# Output compilers and linker.
message(TRACE "c/c++ compiler/linker:")
message(TRACE "==> c compiler: ${CMAKE_C_COMPILER}")
message(TRACE "==> c++ compiler: ${CMAKE_CXX_COMPILER}")
message(TRACE "==> linker: ${CMAKE_LINKER}")
message(TRACE "==> stdc lib dir: ${STDC_LIB_DIR}")

# Build type && Configuration types detect.
if (CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Multi-config generator, configuration types: ${CMAKE_CONFIGURATION_TYPES}")
else()
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type(for single-config generator)" FORCE)
    endif()
    message(STATUS "Single-config generator, build type: ${CMAKE_BUILD_TYPE}")
endif()

# Debug/asan target file suffix (mirror premake targetsuffix: _debug / _asan / _asan_debug).
# Note: 
# - Apply notes:
#           Var						|    cmake supported  | lib target | exe target
#      CMAKE_DEBUG_POSTFIX          |           Y	      |     Y      |     N
#      CMAKE_RELEASE_POSTFIX        |           N	      |     N      |     N
#   CMAKE_RELWITHDEBINFO_POSTFIX    |           N	      |     N      |     N
#     CMAKE_MINSIZEREL_POSTFIX      |           N	      |     N      |     N
if (LLBC_ENABLE_ASAN)
	set(CMAKE_DEBUG_POSTFIX _asan_debug)
	set(CMAKE_RELEASE_POSTFIX _asan)
	set(CMAKE_RELWITHDEBINFO_POSTFIX _asan)
	set(CMAKE_MINSIZEREL_POSTFIX _asan)
else()
	set(CMAKE_DEBUG_POSTFIX _debug)
endif()

# Set project cmake options.
set(CMAKE_COLOR_MAKEFILE ON CACHE BOOL "Enable color makefile" FORCE)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands" FORCE)

# Set archive/library/runtime output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Disable min/max macro define(on MSVC compiler).
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

# Generate LLBC_VERSION.
set(LLBC_PY_CFG_FILE_PATH "${LLBC_TOP_DIR}/tools/build_scripts/com/cfg.py")
file(STRINGS
	 "${LLBC_PY_CFG_FILE_PATH}"
	 LLBC_VERSION_LINES
	 REGEX "^[ \t]*LIB_VER[ \t]*=[ \t]*'([0-9]+\\.[0-9]+\\.[0-9]+)'")
if (NOT LLBC_VERSION_LINES)
    message(FATAL_ERROR "LLBC_VER variable not found in cfg.py, dir: ${CMAKE_SOURCE_DIR}")
endif()

list(GET LLBC_VERSION_LINES 0 LLBC_VERSION_LINE)
string(REGEX MATCH "'([0-9]+\\.[0-9]+\\.[0-9]+)'" _ "${LLBC_VERSION_LINE}")
set(LLBC_VERSION "${CMAKE_MATCH_1}")
if (NOT LLBC_VERSION)
	message(FATAL_ERROR "Failed to get llbc framework version")
endif()

string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" _ "${LLBC_VERSION}")
set(LLBC_MAJOR_VERSION ${CMAKE_MATCH_1})
set(LLBC_MINOR_VERSION ${CMAKE_MATCH_2})
set(LLBC_UPDATE_NUMBER ${CMAKE_MATCH_3})
if (NOT LLBC_MAJOR_VERSION OR NOT LLBC_MINOR_VERSION OR NOT LLBC_UPDATE_NUMBER)
    message(FATAL_ERROR "LLBC_VER version config illegal")
endif()

message(STATUS "llbc framework version: ${LLBC_VERSION}")

# Common build settings carrier (mirror premake workspace-level c++ settings).
# Only llbc's own targets link this; 3rdparty(googletest) must NOT inherit -Werror etc.
add_library(llbc_sln_build_settings INTERFACE)
if (NOT MSVC)
	target_compile_options(llbc_sln_build_settings INTERFACE -Wall -Werror -Wno-strict-aliasing)
endif()

# DEBUG macro on debug builds (mirror premake "configurations:debug*" defines { "DEBUG" }).
target_compile_definitions(llbc_sln_build_settings INTERFACE $<$<CONFIG:Debug>:DEBUG>)

# CXX11 ABI selection (non-msvc compiler).
if (NOT MSVC)
	if (LLBC_DISABLE_CXX11_ABI)
		target_compile_definitions(llbc_sln_build_settings INTERFACE _GLIBCXX_USE_CXX11_ABI=0)
	else()
		target_compile_definitions(llbc_sln_build_settings INTERFACE _GLIBCXX_USE_CXX11_ABI=1)
	endif()
endif()

# Export dynamic symbol table for backtraces (mirror premake linkoptions { "-rdynamic" }).
if (NOT MSVC)
	target_link_options(llbc_sln_build_settings INTERFACE -rdynamic)
endif()

# macOS: Foundation framework(.mm sources) + rpath so shared-linked exes find the dylib in the flat output dir.
if (APPLE)
	target_link_options(llbc_sln_build_settings INTERFACE "SHELL:-framework Foundation")
	target_link_options(llbc_sln_build_settings INTERFACE "SHELL:-Wl,-rpath,@loader_path")
endif()

# AddressSanitizer (non-Windows).
if (LLBC_ENABLE_ASAN AND NOT WIN32)
	target_compile_options(llbc_sln_build_settings INTERFACE -fsanitize=address -g)
	target_link_options(llbc_sln_build_settings INTERFACE -fsanitize=address)
endif()

# Coverage: clang source-based coverage instrumentation (non-MSVC). Because both the core lib and
# unit_test link llbc_sln_build_settings, this instruments the module .cpp (in the lib) AND the inline
# overloads (*Inl.h) compiled into the test TU. googletest does not inherit these flags.
if (LLBC_ENABLE_COVERAGE AND NOT MSVC)
	target_compile_options(llbc_sln_build_settings INTERFACE -fprofile-instr-generate -fcoverage-mapping)
	target_link_options(llbc_sln_build_settings INTERFACE -fprofile-instr-generate -fcoverage-mapping)
endif()

# Function: Apply config postfix to all targets.
function(llbc_apply_config_postfix)
    foreach(_tgt IN LISTS ARGN)
        set_target_properties(${_tgt} PROPERTIES
            DEBUG_POSTFIX          "${CMAKE_DEBUG_POSTFIX}"
            RELEASE_POSTFIX        "${CMAKE_RELEASE_POSTFIX}"
            RELWITHDEBINFO_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}"
            MINSIZEREL_POSTFIX     "${CMAKE_MINSIZEREL_POSTFIX}")
    endforeach()
endfunction() 
