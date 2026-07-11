project(llbc_func_test)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_FUNC_TEST_SRC_FILES *.cpp *.h)

# Include core lib directory.
set(LLBC_CORE_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
link_directories(${LLBC_OUTPUT_DIR})

# Include func test directory.
include_directories(${LLBC_LIB_FUNC_TEST_DIR})

# Set func test dependency libraries.
if(WIN32)
    set(LLBC_LIB_FUNC_TEST_STATIC_LIB ws2_32 Mswsock DbgHelp llbc_lib)
    set(LLBC_LIB_FUNC_TEST_SHARED_LIB ws2_32 Mswsock DbgHelp llbc_lib_shared)
elseif(UNIX AND NOT APPLE)
    set(LLBC_LIB_FUNC_TEST_STATIC_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64 llbc_lib -lunwind -lunwind-x86_64)
    set(LLBC_LIB_FUNC_TEST_SHARED_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64 llbc_lib_shared -lunwind -lunwind-x86_64)
elseif(APPLE)
    set(LLBC_LIB_FUNC_TEST_STATIC_LIB -liconv llbc_lib)
    set(LLBC_LIB_FUNC_TEST_SHARED_LIB llbc_lib_shared)
endif()

# Set func test output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

# Copy all func test config file to output directory.
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/config/test_ini.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/config/test_prop.properties DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/core/log/LogTestCfg.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.cfg DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.properties DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.ini DESTINATION ${LLBC_OUTPUT_DIR})
file(COPY ${LLBC_LIB_FUNC_TEST_DIR}/app/AppCfgTest.xml DESTINATION ${LLBC_OUTPUT_DIR})

# Build func test.
set(LLBC_LIB_FUNC_TEST_LINK_STATIC_LIB ${PROJECT_NAME})
add_executable(${LLBC_LIB_FUNC_TEST_LINK_STATIC_LIB} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_STATIC_LIB} ${LLBC_LIB_FUNC_TEST_STATIC_LIB})
target_compile_definitions(${LLBC_LIB_FUNC_TEST_LINK_STATIC_LIB} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)

set(LLBC_LIB_FUNC_TEST_LINK_SHARED_LIB ${PROJECT_NAME}_shared_lib)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_SHARED_LIB} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_SHARED_LIB} ${LLBC_LIB_FUNC_TEST_SHARED_LIB})
target_compile_definitions(${LLBC_LIB_FUNC_TEST_LINK_SHARED_LIB} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)

