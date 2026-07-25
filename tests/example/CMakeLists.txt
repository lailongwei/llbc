# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_EXAMPLE_SRC_FILES *.h *.cpp)

# Group source files.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LLBC_LIB_EXAMPLE_SRC_FILES})

# Build link static llbc library executable.
set(LLBC_LIB_EXAMPLE_LINK_STATIC_EXE example_static)
add_executable(${LLBC_LIB_EXAMPLE_LINK_STATIC_EXE} ${LLBC_LIB_EXAMPLE_SRC_FILES})
target_include_directories(${LLBC_LIB_EXAMPLE_LINK_STATIC_EXE}
                           PRIVATE ${LLBC_LIB_EXAMPLE_DIR})
target_link_libraries(${LLBC_LIB_EXAMPLE_LINK_STATIC_EXE}
                      PRIVATE llbc_build_settings llbc_lib_static)
llbc_apply_config_postfix(${LLBC_LIB_EXAMPLE_LINK_STATIC_EXE})
set_target_properties(${LLBC_LIB_EXAMPLE_LINK_STATIC_EXE} PROPERTIES FOLDER "tests")

# Build link shared llbc library executable.
set(LLBC_LIB_EXAMPLE_LINK_SHARED_EXE example_shared)
add_executable(${LLBC_LIB_EXAMPLE_LINK_SHARED_EXE} ${LLBC_LIB_EXAMPLE_SRC_FILES})
target_include_directories(${LLBC_LIB_EXAMPLE_LINK_SHARED_EXE}
                           PRIVATE ${LLBC_LIB_EXAMPLE_DIR})
target_link_libraries(${LLBC_LIB_EXAMPLE_LINK_SHARED_EXE}
                      PRIVATE llbc_build_settings llbc_lib_shared)
llbc_apply_config_postfix(${LLBC_LIB_EXAMPLE_LINK_SHARED_EXE})
set_target_properties(${LLBC_LIB_EXAMPLE_LINK_SHARED_EXE} PROPERTIES
                      FOLDER "tests"
                      OUTPUT_NAME "example")