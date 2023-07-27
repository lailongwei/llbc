# Set project name.
project(llbc_core_lib)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_SRC_FILES *.cpp)

# Include core lib directory.
SET(LLBC_CORE_LIB_INCLUDE ${LLBC_CORE_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})

# Link core lib dependency library.
IF(WIN32)
    set(LLBC_CORE_LIB_LINK_LIBRARIES ws2_32 Mswsock DbgHelp)
ELSEIF(UNIX AND NOT APPLE)
    set(LLBC_CORE_LIB_LINK_LIBRARIES -ldl -lpthread -luuid)
ELSEIF(APPLE)
    set(LLBC_CORE_LIB_LINK_LIBRARIES -liconv)
ENDIF()

# Set core library output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build core library static library.
set(LLBC_CORE_LIB_STATIC ${PROJECT_NAME})
add_library(${LLBC_CORE_LIB_STATIC} STATIC ${LLBC_SRC_FILES})
target_link_libraries(${LLBC_CORE_LIB_STATIC})
target_compile_definitions(${LLBC_CORE_LIB_STATIC} PRIVATE -DLLBC_CORE_LIB_STATIC)

# Build core library shared library.
set(LLBC_CORE_LIB_SHARED ${PROJECT_NAME}_shared)
add_library(${LLBC_CORE_LIB_SHARED} SHARED ${LLBC_SRC_FILES})
target_link_libraries(${LLBC_CORE_LIB_SHARED} ${LLBC_CORE_LIB_LINK_LIBRARIES})
target_compile_definitions(${LLBC_CORE_LIB_SHARED} PRIVATE -DLLBC_CORE_LIB_SHARED)