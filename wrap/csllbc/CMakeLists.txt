# Collect csllbc native library sources.
file(GLOB_RECURSE CSLLBC_NATIVE_SOURCES native/*.h native/*.cpp)
# Collect csllbc library sources.
file(GLOB_RECURSE CSLLBC_CS_SOURCES csharp/*.cs)
file(GLOB_RECURSE CSLLBC_PY_SOURCES csharp/*.py)
# Collect csllbc testsuite sources.
file(GLOB_RECURSE CSLLBC_TESTSUITE_SOURCES testsuite/*.cs)

# Group source files.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/native FILES ${CSLLBC_NATIVE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/csharp FILES ${CSLLBC_CS_SOURCES} ${CSLLBC_PY_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/testsuite FILES ${CSLLBC_TESTSUITE_SOURCES})

# Build csllbc native library(shared).
set(LLBC_WRAP_LIB_CSLLBC_NATIVE csllbc_native)
add_library(${LLBC_WRAP_LIB_CSLLBC_NATIVE} SHARED ${CSLLBC_NATIVE_SOURCES})
target_include_directories(${LLBC_WRAP_LIB_CSLLBC_NATIVE}
                           PRIVATE ${LLBC_WRAP_DIR_CSLLBC}/native/include)
target_link_libraries(${LLBC_WRAP_LIB_CSLLBC_NATIVE}
                      PRIVATE llbc_build_settings ${LLBC_LIB_SHARED})
set_target_properties(${LLBC_WRAP_LIB_CSLLBC_NATIVE} PROPERTIES
                      FOLDER "wrap/csllbc")
if (WIN32)
    set_target_properties(${LLBC_WRAP_LIB_CSLLBC_NATIVE} PROPERTIES
                          PREFIX "lib"
                          ARCHIVE_OUTPUT_NAME "lib${LLBC_WRAP_LIB_CSLLBC_NATIVE}")
endif()

# Build csllbc library(c#).
set(LLBC_WRAP_LIB_CSLLBC csllbc)
set_source_files_properties(${CSLLBC_CS_SOURCES} PROPERTIES LANGUAGE CSharp)
set_source_files_properties(${CSLLBC_PY_SOURCES} PROPERTIES
                            VS_TOOL_OVERRIDE "None"
                            HEADER_FILE_ONLY TRUE)
add_library(${LLBC_WRAP_LIB_CSLLBC} SHARED ${CSLLBC_CS_SOURCES} ${CSLLBC_PY_SOURCES})
set_target_properties(${LLBC_WRAP_LIB_CSLLBC} PROPERTIES
                      LINKER_LANGUAGE CSharp
                      VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
                      VS_DOTNET_REFERENCES "System;System.Net;System.Core"
                      VS_GLOBAL_AllowUnsafeBlocks "true"
                      FOLDER "wrap/csllbc")
add_dependencies(${LLBC_WRAP_LIB_CSLLBC} ${LLBC_WRAP_LIB_CSLLBC_NATIVE})

# csllbc native library pre-build commands: generate P/Invoke code & errno code.
add_custom_command(TARGET ${LLBC_WRAP_LIB_CSLLBC}
                   PRE_BUILD
                   COMMAND ${Python_EXECUTABLE} "${LLBC_WRAP_DIR_CSLLBC}/csharp/script_tools/gen_native_code.py"
                   COMMAND ${Python_EXECUTABLE} "${LLBC_WRAP_DIR_CSLLBC}/csharp/script_tools/gen_errno_code.py"
                   COMMENT "Pre-build ${LLBC_WRAP_LIB_CSLLBC}: Generate P/Invoke code & errno code")

# csllbc native library post-build commands: (Non-Win32 spec)generate dll mapping xml file.
if (NOT WIN32)
    add_custom_command(TARGET ${LLBC_WRAP_LIB_CSLLBC}
                       POST_BUILD
                       COMMAND ${Python_EXECUTABLE} "${LLBC_WRAP_DIR_CSLLBC}/csharp/gen_dll_cfg.py" ${LLBC_OUTPUT_DIR}
                       COMMENT "Post build ${LLBC_WRAP_LIB_CSLLBC}: Generate dll mapping xml file")
endif()

# Build csllbc test suite(c#).
set(LLBC_WRAP_LIB_CSLLBC_TESTSUITE ${LLBC_WRAP_LIB_CSLLBC}_testsuite)
set_source_files_properties(${CSLLBC_TESTSUITE_SOURCES} PROPERTIES LANGUAGE CSharp)
add_executable(${LLBC_WRAP_LIB_CSLLBC_TESTSUITE} ${CSLLBC_TESTSUITE_SOURCES})
target_link_libraries(${LLBC_WRAP_LIB_CSLLBC_TESTSUITE}
                      PRIVATE ${LLBC_WRAP_LIB_CSLLBC})
set_target_properties(${LLBC_WRAP_LIB_CSLLBC_TESTSUITE} PROPERTIES
                      LINKER_LANGUAGE CSharp
                      VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
                      VS_DOTNET_REFERENCES "System;System.Net;System.Core"
                      FOLDER "wrap/csllbc")

# Set targets postfix.
llbc_apply_config_postfix(${LLBC_WRAP_LIB_CSLLBC_NATIVE} ${LLBC_WRAP_LIB_CSLLBC} ${LLBC_WRAP_LIB_CSLLBC_TESTSUITE})
