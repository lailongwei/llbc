project(llbc_unit_test)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_UNIT_TEST_SRC_FILES *.cpp *.h)

# Include core lib directory.
set(LLBC_CORE_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
link_directories(${LLBC_OUTPUT_DIR})

# Include gtest directory.
set(LLBC_GTEST_DIR ${LLBC_LIB_TESTS_DIR}/3rdparty/googletest)
include_directories(${LLBC_GTEST_DIR}/googletest/include)
link_directories(${LLBC_GTEST_DIR}/build/lib)

# Define custom target: compile googletest framework.
add_custom_target(
    build_googletest
    COMMAND echo "Building googletest..."
    COMMAND mkdir -p ${LLBC_GTEST_DIR}/build
    COMMAND cd ${LLBC_GTEST_DIR}/build && cmake .. && make
    DEPENDS ${LLBC_GTEST_DIR}/CMakeLists.txt
)

# Set unit test dependency libraries.
if(WIN32)
    set(LLBC_LIB_UNIT_TEST_STATIC_LIB ws2_32 Mswsock DbgHelp llbc_lib)
    set(LLBC_LIB_UNIT_TEST_SHARED_LIB ws2_32 Mswsock DbgHelp llbc_lib_shared)
elseif(UNIX AND NOT APPLE)
    set(LLBC_LIB_UNIT_TEST_STATIC_LIB gtest -ldl -lpthread -luuid llbc_lib -lunwind -lunwind-x86_64)
    set(LLBC_LIB_UNIT_TEST_SHARED_LIB gtest -ldl -lpthread -luuid llbc_lib_shared -lunwind -lunwind-x86_64)
elseif(APPLE)
    set(LLBC_LIB_UNIT_TEST_STATIC_LIB -liconv -lgtest llbc_lib)
    set(LLBC_LIB_UNIT_TEST_SHARED_LIB -lgtest llbc_lib_shared)
endif()

# Include unit test directory.
include_directories(${LLBC_LIB_UNIT_TEST_DIR})

# Set unit test output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build unit test.
set(LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE ${PROJECT_NAME})
add_executable(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} ${LLBC_LIB_UNIT_TEST_SRC_FILES})
add_dependencies(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} build_googletest)
target_link_libraries(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} ${LLBC_LIB_UNIT_TEST_STATIC_LIB})
target_compile_definitions(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)

set(LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE ${PROJECT_NAME}_shared)
add_executable(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} ${LLBC_LIB_UNIT_TEST_SRC_FILES})
add_dependencies(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} build_googletest)
target_link_libraries(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} ${LLBC_LIB_UNIT_TEST_SHARED_LIB})
target_compile_definitions(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)

