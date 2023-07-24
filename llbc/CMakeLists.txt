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

ELIF(UNIX AND NOT APPLE)
    SET(LLBC_CORE_LIB_DEPS,
            dl,
            uuid,
            pthread)

ELIF(APPLE)
    SET(LLBC_CORE_LIB_DEPS
            iconv)

ENDIF()

# Set core library static library output directory.
SET(LLBC_LIB_OUTPUT_DIR ${LLBC_OUTPUT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LLBC_LIB_OUTPUT_DIR})

# Build core library static library.
ADD_LIBRARY(${PROJECT_NAME} STATIC ${LLBC_SRC_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LLBC_CORE_LIB_DEPS})