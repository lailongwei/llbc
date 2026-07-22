# Recurse all source files.
file(GLOB_RECURSE LLBC_LIB_COMMON_SRC_FILES *.cpp)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB_RECURSE LLBC_LIB_PLATFORM_SRC_FILES *.mm)
endif()
set(SOURCES ${LLBC_LIB_COMMON_SRC_FILES} ${LLBC_LIB_PLATFORM_SRC_FILES})

# Include llbc lib directory.
set(LLBC_LIB_INCLUDE_DIR "${LLBC_LIB_DIR}/include")

# Link llbc lib dependency library.
if(WIN32)
    set(LLBC_DEP_LIBS ws2_32 Mswsock DbgHelp)
elseif(APPLE)
    set(LLBC_DEP_LIBS iconv)
else()
    set(LLBC_DEP_LIBS dl pthread uuid unwind unwind-x86_64)
endif()

# Build static library.
set(LLBC_LIB_STATIC ${PROJECT_NAME}_lib_static)
add_library(${LLBC_LIB_STATIC} STATIC ${SOURCES})
target_include_directories(${LLBC_LIB_STATIC}
                           PUBLIC "${LLBC_LIB_DIR}/include")
target_compile_definitions(${LLBC_LIB_STATIC}
                           PRIVATE LLBC_LIB_STATIC)
target_link_libraries(${LLBC_LIB_STATIC}
                      PRIVATE llbc_sln_build_settings 
                      INTERFACE ${LLBC_DEP_LIBS})

# Build shared library.
set(LLBC_LIB_SHARED ${PROJECT_NAME}_lib_shared)
add_library(${LLBC_LIB_SHARED} SHARED ${SOURCES})
target_include_directories(${LLBC_LIB_SHARED}
                           PUBLIC "${LLBC_LIB_DIR}/include")
target_compile_definitions(${LLBC_LIB_SHARED}
                           PRIVATE LLBC_LIB_SHARED)
target_link_libraries(${LLBC_LIB_SHARED}
                      PRIVATE llbc_sln_build_settings
                      PUBLIC ${LLBC_DEP_LIBS})

# Set output name.
if (WIN32)
    set_target_properties(${LLBC_LIB_STATIC} PROPERTIES OUTPUT_NAME ${LLBC_LIB_STATIC})
    set_target_properties(${LLBC_LIB_SHARED} PROPERTIES OUTPUT_NAME ${LLBC_LIB_SHARED})
else()
    set_target_properties(${LLBC_LIB_STATIC} ${LLBC_LIB_SHARED} PROPERTIES OUTPUT_NAME llbc)
endif()
