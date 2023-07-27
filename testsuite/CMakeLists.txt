project(llbc_testsuite)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_SRC_FILES *.cpp *.h)

# Include core lib directory.
SET(LLBC_CORE_LIB_INCLUDE ${LLBC_CORE_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
link_directories(${LLBC_OUTPUT_DIR})

# Include testsuite directory.
include_directories(${LLBC_TESTSUITE_DIR})

# Set testsuite dependency libraries.
IF(WIN32)
    set(LLBC_TESTSUITE_LINK_STATIC_LIBRARIES ws2_32 Mswsock DbgHelp llbc_core_lib)
    set(LLBC_TESTSUITE_LINK_SHARED_LIBRARIES ws2_32 Mswsock DbgHelp llbc_core_lib_shared)
ELSEIF(UNIX AND NOT APPLE)
    set(LLBC_TESTSUITE_LINK_STATIC_LIBRARIES -ldl -lpthread -luuid llbc_core_lib)
    set(LLBC_TESTSUITE_LINK_SHARED_LIBRARIES ws2_32 Mswsock DbgHelp llbc_core_lib_shared)
ELSEIF(APPLE)
    set(LLBC_TESTSUITE_LINK_STATIC_LIBRARIES -liconv llbc_core_lib)
    set(LLBC_TESTSUITE_LINK_SHARED_LIBRARIES ws2_32 Mswsock DbgHelp llbc_core_lib_shared)
ENDIF()

# Set testsuite output directory.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build testsuite.
SET(LLBC_TESTSUITE_LINK_STATIC_LIBRARY ${PROJECT_NAME})
add_executable(${LLBC_TESTSUITE_LINK_STATIC_LIBRARY} ${LLBC_SRC_FILES})
target_link_libraries(${LLBC_TESTSUITE_LINK_STATIC_LIBRARY} ${LLBC_TESTSUITE_LINK_STATIC_LIBRARIES})
target_compile_definitions(${LLBC_TESTSUITE_LINK_STATIC_LIBRARY} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)

SET(LLBC_TESTSUITE_LINK_SHARED_LIBRARY ${PROJECT_NAME}_shared_lib)
add_executable(${LLBC_TESTSUITE_LINK_SHARED_LIBRARY} ${LLBC_SRC_FILES})
target_link_libraries(${LLBC_TESTSUITE_LINK_SHARED_LIBRARY} ${LLBC_TESTSUITE_LINK_SHARED_LIBRARIES})
target_compile_definitions(${LLBC_TESTSUITE_LINK_SHARED_LIBRARY} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)

# Copy all testcase config file to output directory.
file(COPY ${LLBC_TESTSUITE_DIR}/core/config/IniTestCfg.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/core/log/LogTestCfg.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.xml DESTINATION ${LLBC_OUTPUT_DIR})
