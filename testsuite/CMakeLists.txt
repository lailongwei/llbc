project(llbc_testsuite)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_SRC_FILES *.cpp *.h)

# Include core lib directory.
SET(LLBC_CORE_LIB_INCLUDE ${LLBC_CORE_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
include_directories(${LLBC_TEST_DIR})

# Link core lib library.
link_directories(${LLBC_LIB_OUTPUT_DIR})

# Set testsuite dependency libraries.
IF(WIN32)
    SET(LLBC_TESTSUITE_DEP_LIBS
            ws2_32.lib,
            Mswsock.lib,
            DbgHelp.lib)
ELIF(UNIX AND NOT APPLE)
    SET(LLBC_TESTSUITE_DEP_LIBS
            dl,
            uuid,
            pthread,
            llbc_core_lib)
ELSEIF(APPLE)
    SET(LLBC_TESTSUITE_DEP_LIBS
            llbc_core_lib
            iconv)
ENDIF()

# Set testsuite output directory.
SET(LLBC_TESTSUITE_OUTPUT_DIR ${LLBC_OUTPUT_DIR}/testsuite)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_TESTSUITE_OUTPUT_DIR})

# Build testsuite.
add_executable(${PROJECT_NAME} ${LLBC_SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${LLBC_TESTSUITE_DEP_LIBS})