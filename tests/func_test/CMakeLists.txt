project(llbc_func_test)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_FUNC_TEST_SRC_FILES *.cpp)

# Collect all func test config files.
file(GLOB_RECURSE LLBC_LIB_FUNC_TEST_CFG_FILES
     CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cfg
     ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
     ${CMAKE_CURRENT_SOURCE_DIR}/*.ini
     ${CMAKE_CURRENT_SOURCE_DIR}/*.properties)
# Copy func test config files to output dir at build time.
add_custom_target(llbc_func_test_copy_cfg
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LLBC_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LLBC_LIB_FUNC_TEST_CFG_FILES} "${LLBC_OUTPUT_DIR}/"
    COMMENT "Copying func_test config files to ${LLBC_OUTPUT_DIR}"
    VERBATIM)

# Build link static llbc library executable.
set(LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE ${PROJECT_NAME}_static)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                           PRIVATE ${LLBC_LIB_FUNC_TEST_DIR})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                      PRIVATE llbc_sln_build_settings llbc_lib_static)
llbc_apply_config_postfix(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE})
add_dependencies(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} llbc_func_test_copy_cfg)

# Build link shared llbc library executable.
set(LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE ${PROJECT_NAME}_shared)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                           PRIVATE ${LLBC_LIB_FUNC_TEST_DIR})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                      PRIVATE llbc_sln_build_settings llbc_lib_shared)
llbc_apply_config_postfix(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE})
add_dependencies(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} llbc_func_test_copy_cfg)
