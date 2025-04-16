# Set project name.
project(llbc_lib)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_LIB_SRC_FILES *.cpp)

# Include llbc lib directory.
set(LLBC_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_LIB_INCLUDE})

# Link llbc lib dependency library.
if(WIN32)
    set(LLBC_LIB_LINK_LIB ws2_32 Mswsock DbgHelp)
elseif(UNIX AND NOT APPLE)
    set(LLBC_LIB_LINK_LIB -ldl -lpthread -luuid -lunwind -unwind-x86_64)
elseif(APPLE)
    set(LLBC_LIB_LINK_LIB -liconv)
endif()

# Set llbc library output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build core library static library.
set(LLBC_LIB_STATIC ${PROJECT_NAME})
add_library(${LLBC_LIB_STATIC} STATIC ${LLBC_LIB_SRC_FILES})
target_link_libraries(${LLBC_LIB_STATIC})
target_compile_definitions(${LLBC_LIB_STATIC} PRIVATE -DLLBC_LIB_STATIC)

# Build core library shared library.
set(LLBC_LIB_SHARED ${PROJECT_NAME}_shared)
add_library(${LLBC_LIB_SHARED} SHARED ${LLBC_LIB_SRC_FILES})
target_link_libraries(${LLBC_LIB_SHARED} ${LLBC_LIB_LINK_LIB})
target_compile_definitions(${LLBC_LIB_SHARED} PRIVATE -DLLBC_LIB_SHARED)
