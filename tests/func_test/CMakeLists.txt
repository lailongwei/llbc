# Collect all .h/.cpp files.
file(GLOB_RECURSE LLBC_LIB_FUNC_TEST_SRC_FILES *.h *.cpp)

# Group source files.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LLBC_LIB_FUNC_TEST_SRC_FILES})

# Build link static llbc library executable.
set(LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE func_test_static)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                           PRIVATE ${LLBC_LIB_FUNC_TEST_DIR})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                      PRIVATE llbc_build_settings ${LLBC_LIB_SHARED})
llbc_apply_config_postfix(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE})
set_target_properties(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} PROPERTIES
                      FOLDER "tests"
                      VS_DEBUGGER_WORKING_DIRECTORY "${LLBC_OUTPUT_DIR}")

# Build link shared llbc library executable.
set(LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE func_test_shared)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_include_directories(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                           PRIVATE ${LLBC_LIB_FUNC_TEST_DIR})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                      PRIVATE llbc_build_settings ${LLBC_LIB_SHARED})
llbc_apply_config_postfix(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE})
set_target_properties(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} PROPERTIES
                      FOLDER "tests"
                      OUTPUT_NAME "func_test"
                      VS_DEBUGGER_WORKING_DIRECTORY "${LLBC_OUTPUT_DIR}")

# Set startup project to func_test_shared: handled at top-level CMakeLists.txt
# (VS_STARTUP_PROJECT must be set on the top-level source dir).

# Collect all func test config files.
file(GLOB_RECURSE LLBC_LIB_FUNC_TEST_CFG_FILES
     CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cfg
     ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
     ${CMAKE_CURRENT_SOURCE_DIR}/*.ini
     ${CMAKE_CURRENT_SOURCE_DIR}/*.properties)

# Post-Build: Copy func test config files to output dir at build time.
add_custom_command(TARGET ${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory "${LLBC_OUTPUT_DIR}"
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LLBC_LIB_FUNC_TEST_CFG_FILES} "${LLBC_OUTPUT_DIR}/"
                   COMMENT "Copying func_test config files to ${LLBC_OUTPUT_DIR}"
                   VERBATIM)
add_custom_command(TARGET ${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory "${LLBC_OUTPUT_DIR}"
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LLBC_LIB_FUNC_TEST_CFG_FILES} "${LLBC_OUTPUT_DIR}/"
                   COMMENT "Copying func_test config files to ${LLBC_OUTPUT_DIR}"
                   VERBATIM)

