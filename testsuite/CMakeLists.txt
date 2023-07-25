project(llbc_testsuite)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_SRC_FILES *.cpp *.h)

# Include core lib directory.
SET(LLBC_CORE_LIB_INCLUDE ${LLBC_CORE_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
# Include testsuite directory.
include_directories(${LLBC_TESTSUITE_DIR})

# Set testsuite dependency libraries.
IF(WIN32)
    SET(LLBC_TESTSUITE_DEP_LIBS
            ws2_32.lib,
            Mswsock.lib,
            DbgHelp.lib)
ELSEIF(UNIX AND NOT APPLE)
    SET(LLBC_TESTSUITE_DEP_LIBS
            dl,
            uuid,
            pthread,
            llbc_core_lib)
ELSEIF(APPLE)
    SET(LLBC_TESTSUITE_DEP_LIBS
            llbc_core_lib)
ENDIF()

# Copy all testcase config file to output directory.
configure_file(${LLBC_TESTSUITE_DIR}/core/config/IniTestCfg.ini ${LLBC_OUTPUT_DIR} COPYONLY)
configure_file(${LLBC_TESTSUITE_DIR}/core/log/LogTestCfg.cfg ${LLBC_OUTPUT_DIR} COPYONLY)
configure_file(${LLBC_TESTSUITE_DIR}/app/AppCfgTest.cfg ${LLBC_OUTPUT_DIR} COPYONLY)
configure_file(${LLBC_TESTSUITE_DIR}/app/AppCfgTest.ini ${LLBC_OUTPUT_DIR} COPYONLY)
configure_file(${LLBC_TESTSUITE_DIR}/app/AppCfgTest.xml ${LLBC_OUTPUT_DIR} COPYONLY)

# Set testsuite output directory.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build testsuite.
add_executable(${PROJECT_NAME} ${LLBC_SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${LLBC_TESTSUITE_DEP_LIBS})
