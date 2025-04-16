project(llbc_testsuite)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_TEST_SRC_FILES *.cpp *.h)

# Include core lib directory.
set(LLBC_CORE_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
link_directories(${LLBC_OUTPUT_DIR})

# Include testsuite directory.
include_directories(${LLBC_TESTSUITE_DIR})

# Set testsuite dependency libraries.
if(WIN32)
    set(LLBC_TESTSUITE_STATIC_LIB ws2_32 Mswsock DbgHelp llbc_lib)
    set(LLBC_TESTSUITE_SHARED_LIB ws2_32 Mswsock DbgHelp llbc_lib_shared)
elseif(UNIX AND NOT APPLE)
    set(LLBC_TESTSUITE_STATIC_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64 llbc_lib)
    set(LLBC_TESTSUITE_SHARED_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64 llbc_lib_shared)
elseif(APPLE)
    set(LLBC_TESTSUITE_STATIC_LIB -liconv llbc_lib)
    set(LLBC_TESTSUITE_SHARED_LIB llbc_lib_shared)
endif()

# Set testsuite output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

# Copy all testcase config file to output directory.
file(COPY ${LLBC_TESTSUITE_DIR}/core/config/test_ini.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/core/config/test_prop.properties DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/core/log/LogTestCfg.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.properties DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_TESTSUITE_DIR}/app/AppCfgTest.xml DESTINATION ${LLBC_OUTPUT_DIR})

# Build testsuite.
set(LLBC_TESTSUITE_LINK_STATIC_LIB ${PROJECT_NAME})
add_executable(${LLBC_TESTSUITE_LINK_STATIC_LIB} ${LLBC_TEST_SRC_FILES})
target_link_libraries(${LLBC_TESTSUITE_LINK_STATIC_LIB} ${LLBC_TESTSUITE_STATIC_LIB})
target_compile_definitions(${LLBC_TESTSUITE_LINK_STATIC_LIB} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)

set(LLBC_TESTSUITE_LINK_SHARED_LIB ${PROJECT_NAME}_shared_lib)
add_executable(${LLBC_TESTSUITE_LINK_SHARED_LIB} ${LLBC_TEST_SRC_FILES})
target_link_libraries(${LLBC_TESTSUITE_LINK_SHARED_LIB} ${LLBC_TESTSUITE_SHARED_LIB})
target_compile_definitions(${LLBC_TESTSUITE_LINK_SHARED_LIB} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)

