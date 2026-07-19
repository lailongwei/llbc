# CMake minimum version.
cmake_minimum_required (VERSION 3.16)

# Config solution.
include("${CMAKE_SOURCE_DIR}/tools/cmake/config.cmake")

# Set project.
project(llbc VERSION "${LLBC_VERSION}")

# Coverage: clang source-based coverage instrumentation (non-MSVC). Because both the core lib and
# unit_test link llbc_build_flags, this instruments the module .cpp (in the lib) AND the inline
# overloads (*Inl.h) compiled into the test TU. googletest does not inherit these flags.
if (LLBC_ENABLE_COVERAGE AND NOT MSVC)
	target_compile_options(llbc_build_flags INTERFACE -fprofile-instr-generate -fcoverage-mapping)
	target_link_options(llbc_build_flags INTERFACE -fprofile-instr-generate -fcoverage-mapping)
endif()

# Add llbc lib directory.
add_subdirectory(${LLBC_LIB_DIR})
# Add llbc lib test directories.
add_subdirectory(${LLBC_LIB_EXAMPLE_DIR})
add_subdirectory(${LLBC_LIB_FUNC_TEST_DIR})
# if (EXISTS "${LLBC_3RD_DIR_GOOGLETEST}/CMakeLists.txt")
# 	add_subdirectory(${LLBC_LIB_UNIT_TEST_DIR})
# else()
# 	message(NOTICE "3rd - googletest not found(dir: ${LLBC_3RD_DIR_GOOGLETEST}), unit_test will be skipped")
# 	message(NOTICE "- run <git submodule update --init tests/3rdparty/googletest> to update")
# endif()
add_subdirectory(${LLBC_LIB_QUICK_START_DIR})
