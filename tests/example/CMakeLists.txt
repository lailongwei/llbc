project(llbc_example)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_EXAMPLE_SRC_FILES *.cpp *.h)

# Include core lib directory.
set(LLBC_CORE_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})
link_directories(${LLBC_OUTPUT_DIR})

# Include example directory.
include_directories(${LLBC_LIB_EXAMPLE_DIR})

# Set example dependency libraries.
if(WIN32)
    set(LLBC_LIB_EXAMPLE_STATIC_LIB ws2_32 Mswsock DbgHelp llbc_lib)
    set(LLBC_LIB_EXAMPLE_SHARED_LIB ws2_32 Mswsock DbgHelp llbc_lib_shared)
elseif(UNIX AND NOT APPLE)
    set(LLBC_LIB_EXAMPLE_STATIC_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64 llbc_lib -lunwind -lunwind-x86_64)
    set(LLBC_LIB_EXAMPLE_SHARED_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64 llbc_lib_shared -lunwind -lunwind-x86_64)
elseif(APPLE)
    set(LLBC_LIB_EXAMPLE_STATIC_LIB -liconv llbc_lib)
    set(LLBC_LIB_EXAMPLE_SHARED_LIB llbc_lib_shared)
endif()

# Set example output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
file(MAKE_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build example.
set(LLBC_LIB_EXAMPLE_LINK_STATIC_LIB ${PROJECT_NAME})
add_executable(${LLBC_LIB_EXAMPLE_LINK_STATIC_LIB} ${LLBC_LIB_EXAMPLE_SRC_FILES})
target_link_libraries(${LLBC_LIB_EXAMPLE_LINK_STATIC_LIB} ${LLBC_LIB_EXAMPLE_STATIC_LIB})
target_compile_definitions(${LLBC_LIB_EXAMPLE_LINK_STATIC_LIB} PRIVATE -DLLBC_LINK_STATIC_LIBRARY)

set(LLBC_LIB_EXAMPLE_LINK_SHARED_LIB ${PROJECT_NAME}_shared_lib)
add_executable(${LLBC_LIB_EXAMPLE_LINK_SHARED_LIB} ${LLBC_LIB_EXAMPLE_SRC_FILES})
target_link_libraries(${LLBC_LIB_EXAMPLE_LINK_SHARED_LIB} ${LLBC_LIB_EXAMPLE_SHARED_LIB})
target_compile_definitions(${LLBC_LIB_EXAMPLE_LINK_SHARED_LIB} PRIVATE -DLLBC_LINK_SHARED_LIBRARY)

