project(llbc_func_test)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_FUNC_TEST_SRC_FILES *.cpp)

# Copy all func test config file to output directory (mirror premake func_test postbuild copy).
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/config/test_ini.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/config/test_prop.properties DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/log/LogTestCfg.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/log/LogTestCfg.xml DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.properties DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.xml DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgReloadFailedTest.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgReloadFailedTest.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgReloadFailedTest.xml DESTINATION ${LLBC_OUTPUT_DIR})

# Build link static llbc library executable.
set(LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE ${PROJECT_NAME}_static)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                           PRIVATE ${LLBC_LIB_FUNC_TEST_DIR})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                      PRIVATE llbc_sln_build_settings llbc_lib_static)
llbc_apply_config_postfix(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE})

# Build link shared llbc library executable.
set(LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE ${PROJECT_NAME}_shared)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                           PRIVATE ${LLBC_LIB_FUNC_TEST_DIR})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                      PRIVATE llbc_sln_build_settings llbc_lib_shared)
llbc_apply_config_postfix(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE})
