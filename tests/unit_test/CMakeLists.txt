# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_UNIT_TEST_SRC_FILES *.cpp)

# Build link static llbc library executable.
set(LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE unit_test_static)
add_executable(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE} ${LLBC_LIB_UNIT_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE}
                           PRIVATE ${LLBC_LIB_UNIT_TEST_DIR})
target_link_libraries(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE}
                      PRIVATE llbc_sln_build_settings llbc_lib_static GTest::gtest)
llbc_apply_config_postfix(${LLBC_LIB_UNIT_TEST_LINK_STATIC_EXE})

# Build link shared llbc library executable.
set(LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE unit_test_shared)
add_executable(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE} ${LLBC_LIB_UNIT_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE}
                           PRIVATE ${LLBC_LIB_UNIT_TEST_DIR})
target_link_libraries(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE}
                      PRIVATE llbc_sln_build_settings llbc_lib_shared GTest::gtest)
llbc_apply_config_postfix(${LLBC_LIB_UNIT_TEST_LINK_SHARED_EXE})
