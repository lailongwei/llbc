# Set project name.
project(llbc_lib)

# Recurse all source files.
file(GLOB_RECURSE LLBC_LIB_COMMON_SRC_FILES *.cpp)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB_RECURSE LLBC_LIB_PLATFORM_SRC_FILES *.mm)
endif()
set(SOURCES ${LLBC_LIB_COMMON_SRC_FILES} ${LLBC_LIB_PLATFORM_SRC_FILES})

# Include llbc lib directory.
set(LLBC_LIB_INCLUDE ${LLBC_LIB_DIR}/include)
include_directories(${LLBC_LIB_INCLUDE})

# Link llbc lib dependency library.
if(WIN32)
    set(LLBC_LIB_LINK_LIB ws2_32 Mswsock DbgHelp)
elseif(UNIX AND NOT APPLE)
    set(LLBC_LIB_LINK_LIB -ldl -lpthread -luuid -lunwind -lunwind-x86_64)
elseif(APPLE)
    set(LLBC_LIB_LINK_LIB -liconv)
endif()

# Optional: sync version number into src/common/Version.cpp before build (mirror premake llbc_prebuild.py).
# Opt-in only (mutates source tree); enable with -DLLBC_SYNC_VERSION=ON.
if(LLBC_SYNC_VERSION)
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        add_custom_target(llbc_version_sync ALL
            COMMAND ${Python3_EXECUTABLE}
                    ${PROJECT_SOURCE_DIR}/tools/building_script/llbc_prebuild.py
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tools/building_script
            COMMENT "Syncing llbc version into Version.cpp")
    else()
        message(WARNING "LLBC_SYNC_VERSION=ON but Python3 interpreter not found; skipping version sync")
    endif()
endif()

# Set llbc library output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLBC_OUTPUT_DIR})

# Build core library static library.
set(LLBC_LIB_STATIC ${PROJECT_NAME})
add_library(${LLBC_LIB_STATIC} STATIC ${SOURCES})
target_link_libraries(${LLBC_LIB_STATIC} PRIVATE llbc_build_flags)
target_compile_definitions(${LLBC_LIB_STATIC} PRIVATE -DLLBC_LIB_STATIC)

# Build core library shared library.
set(LLBC_LIB_SHARED ${PROJECT_NAME}_shared)
add_library(${LLBC_LIB_SHARED} SHARED ${SOURCES})
target_link_libraries(${LLBC_LIB_SHARED} PRIVATE llbc_build_flags ${LLBC_LIB_LINK_LIB})
target_compile_definitions(${LLBC_LIB_SHARED} PRIVATE -DLLBC_LIB_SHARED)

# Align output artifact name with premake: libllbc.a + libllbc.{so,dylib} (instead of libllbc_lib*).
# Note: naming alignment targets Linux/macOS; on Windows the static/shared import libs would collide
# on the same base name, but Windows uses the Visual Studio/premake path, not this CMake path.
if(NOT WIN32)
    set_target_properties(${LLBC_LIB_STATIC} ${LLBC_LIB_SHARED} PROPERTIES OUTPUT_NAME llbc)
endif()

# Install rules (mirror `make install` -> /usr/lib + /usr/include).
include(GNUInstallDirs)
install(TARGETS ${LLBC_LIB_STATIC} ${LLBC_LIB_SHARED}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${LLBC_LIB_INCLUDE}/llbc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${LLBC_LIB_INCLUDE}/llbc.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
