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
    set(LLBC_LIB_FUNC_TEST_STATIC_LIB -ldl -lpthread -luuid -lunwind -lunwind-x86_64 llbc_lib)
    set(LLBC_LIB_FUNC_TEST_SHARED_LIB -ldl -lpthread -luuid -lunwind -lunwind-x86_64 llbc_lib_shared)
elseif(APPLE)
    set(LLBC_LIB_FUNC_TEST_STATIC_LIB -liconv llbc_lib)
    set(LLBC_LIB_FUNC_TEST_SHARED_LIB llbc_lib_shared)
endif()

# Set func test output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

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

# Build func test (static-linked -> func_test; shared-linked -> func_test_shared; debug adds _debug suffix).
set(LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE ${PROJECT_NAME})
add_executable(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} PRIVATE llbc_build_flags ${LLBC_LIB_FUNC_TEST_STATIC_LIB})
target_compile_definitions(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)
set_target_properties(${LLBC_LIB_FUNC_TEST_LINK_STATIC_EXE} PROPERTIES OUTPUT_NAME func_test
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}" RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}")

set(LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE ${PROJECT_NAME}_shared)
add_executable(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} ${LLBC_LIB_FUNC_TEST_SRC_FILES})
target_link_libraries(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} PRIVATE llbc_build_flags ${LLBC_LIB_FUNC_TEST_SHARED_LIB})
target_compile_definitions(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)
set_target_properties(${LLBC_LIB_FUNC_TEST_LINK_SHARED_EXE} PROPERTIES OUTPUT_NAME func_test_shared
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}" RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
