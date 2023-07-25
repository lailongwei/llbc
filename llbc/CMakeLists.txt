# Set project name.
project(llbc_core_lib)

# Recurse all cpp files.
file(GLOB_RECURSE LLBC_SRC_FILES *.cpp)

# Include core lib directory.
SET(LLBC_CORE_LIB_INCLUDE ${LLBC_CORE_LIB_DIR}/include)
include_directories(${LLBC_CORE_LIB_INCLUDE})

# Link core lib dependency library.
IF(WIN32)
    SET(LLBC_CORE_LIB_DEPS
            ws2_32.lib,
            Mswsock.lib,
            DbgHelp.lib)

ELSEIF(UNIX AND NOT APPLE)
    SET(LLBC_CORE_LIB_DEPS,
            dl,
            uuid,
            pthread)

ELSEIF(APPLE)
    SET(LLBC_CORE_LIB_DEPS
            libiconv.dylib)
ENDIF()

# Set core library output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build core library static library.
ADD_LIBRARY(${PROJECT_NAME} STATIC ${LLBC_SRC_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LLBC_CORE_LIB_DEPS})

# Build core library shared library.
SET(LLBC_CORE_LIB_SHARED_NAME ${PROJECT_NAME}_shared)
ADD_LIBRARY(${LLBC_CORE_LIB_SHARED_NAME} SHARED ${LLBC_SRC_FILES})
TARGET_LINK_LIBRARIES(${LLBC_CORE_LIB_SHARED_NAME} ${LLBC_CORE_LIB_DEPS})