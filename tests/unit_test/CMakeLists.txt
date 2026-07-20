project(llbc_unit_test)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_UNIT_TEST_SRC_FILES *.cpp *.h)

# Include core lib directory.
set(LLBC_CORE_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
link_directories(${LLBC_OUTPUT_DIR})

# Set unit test dependency libraries.
# gtest is provided as a CMake target by add_subdirectory(tests/3rdparty/googletest) in the root
# CMakeLists (it carries its own include dirs), so no manual include/link paths are needed here.
if(WIN32)
    set(LLBC_LIB_UNIT_TEST_STATIC_LIB ws2_32 Mswsock DbgHelp gtest llbc_lib_static)
    set(LLBC_LIB_UNIT_TEST_SHARED_LIB ws2_32 Mswsock DbgHelp gtest llbc_lib_shared)
elseif(UNIX AND NOT APPLE)
    set(LLBC_LIB_UNIT_TEST_STATIC_LIB -ldl -lpthread -luuid -lunwind -lunwind-x86_64 gtest llbc_lib_static)
    set(LLBC_LIB_UNIT_TEST_SHARED_LIB -ldl -lpthread -luuid -lunwind -lunwind-x86_64 gtest llbc_lib_shared)
elseif(APPLE)
    set(LLBC_LIB_UNIT_TEST_STATIC_LIB -liconv gtest llbc_lib_static)
    set(LLBC_LIB_UNIT_TEST_SHARED_LIB gtest llbc_lib_shared)
endif()

# Include unit test directory.
include_directories(${LLBC_LIB_UNIT_TEST_DIR})

# Set unit test output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build unit test (static-linked -> unit_test; shared-linked -> unit_test_shared; debug adds _debug suffix).
set(LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE ${PROJECT_NAME})
add_executable(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} ${LLBC_LIB_UNIT_TEST_SRC_FILES})
target_link_libraries(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} PRIVATE llbc_sln_build_settings ${LLBC_LIB_UNIT_TEST_STATIC_LIB})
target_compile_definitions(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)
set_target_properties(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} PROPERTIES OUTPUT_NAME unit_test
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}" RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}")

set(LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE ${PROJECT_NAME}_shared)
add_executable(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} ${LLBC_LIB_UNIT_TEST_SRC_FILES})
target_link_libraries(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} PRIVATE llbc_sln_build_settings ${LLBC_LIB_UNIT_TEST_SHARED_LIB})
target_compile_definitions(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)
set_target_properties(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} PROPERTIES OUTPUT_NAME unit_test_shared
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}" RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
