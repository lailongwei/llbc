file(GLOB_RECURSE SOURCES include/*.h src/*.cpp)

# Group source files.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Get cpython version.
file(STRINGS "${LLBC_3RD_DIR_CPYTHON}/README"
     _llbc_cpython_version_lines
     REGEX "^This is Python version (.*)$")

     if (NOT _llbc_cpython_version_lines)
    message(FATAL_ERROR "Failed to get 3rd:cpython version")
endif()

list(GET _llbc_cpython_version_lines 0 _llbc_cpython_version_line)
if (_llbc_cpython_version_line MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+)")
    set(LLBC_3RD_CPYTHON_MAJOR_VERSION "${CMAKE_MATCH_1}")
    set(LLBC_3RD_CPYTHON_MINOR_VERSION "${CMAKE_MATCH_2}")
    set(LLBC_3RD_CPYTHON_PATCH_VERSION "${CMAKE_MATCH_3}")
    set(LLBC_3RD_CPYTHON_VERSION "${LLBC_3RD_CPYTHON_MAJOR_VERSION}.${LLBC_3RD_CPYTHON_MINOR_VERSION}.${LLBC_3RD_CPYTHON_PATCH_VERSION}")
    message(STATUS "3rd:cpython version: ${LLBC_3RD_CPYTHON_VERSION}")
else()
    message(FATAL_ERROR "Failed to get 3rd:cpython version")
endif()

# Add custom target: pyllbc_prebuild
add_custom_target(pyllbc_prebuild
                  COMMAND ${Python_EXECUTABLE}
                    "${LLBC_TOOLS_DIR}/build_scripts/py_prebuild.py"
                    ${LLBC_VERSION}
                    ${LLBC_PLATFORM_ARCH} 
                    $<CONFIG>
                    ${LLBC_DISABLE_CXX11_ABI}
                    ${CMAKE_VS_DEVENV_COMMAND}
                  COMMENT "Prebuild pyllbc")
set_target_properties(pyllbc_prebuild PROPERTIES FOLDER "wrap/assistant")

# Get cpython lib output directory & name.
if (WIN32)
    set(LLBC_3RD_CPYTHON_LIB_OUTPUT_DIR "${LLBC_3RD_DIR_CPYTHON}/PCbuild/$<IF:$<STREQUAL:${LLBC_PLATFORM_ARCH},x64>,amd64,${LLBC_PLATFORM_ARCH}>")
    set(LLBC_3RD_CPYTHON_LIB_BASE_NAME "python${LLBC_3RD_CPYTHON_MAJOR_VERSION}${LLBC_3RD_CPYTHON_MINOR_VERSION}")
else()
    set(LLBC_3RD_CPYTHON_LIB_OUTPUT_DIR "${LLBC_3RD_DIR_CPYTHON}")
    set(LLBC_3RD_CPYTHON_LIB_BASE_NAME "python${LLBC_3RD_CPYTHON_MAJOR_VERSION}.${LLBC_3RD_CPYTHON_MINOR_VERSION}")
endif()

# Build pyllbc library(shared).
set(LLBC_WRAP_LIB_PYLLBC pyllbc)
add_library(${LLBC_WRAP_LIB_PYLLBC} SHARED ${SOURCES})
target_include_directories(${LLBC_WRAP_LIB_PYLLBC}
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                           PRIVATE "${LLBC_3RD_DIR_CPYTHON}/Include"
                           PRIVATE "$<IF:$<PLATFORM_ID:Windows>,${LLBC_3RD_DIR_CPYTHON}/PC,${LLBC_3RD_DIR_CPYTHON}>")
target_link_libraries(${LLBC_WRAP_LIB_PYLLBC}
                      PRIVATE llbc_build_settings_loose llbc_lib_shared)
if (WIN32)
    target_link_libraries(${LLBC_WRAP_LIB_PYLLBC}
                          PRIVATE $<IF:$<CONFIG:Debug>,${LLBC_3RD_CPYTHON_LIB_BASE_NAME}_d,${LLBC_3RD_CPYTHON_LIB_BASE_NAME}>)
else()
    target_link_libraries(${LLBC_WRAP_LIB_PYLLBC}
                          PRIVATE ${LLBC_3RD_CPYTHON_LIB_BASE_NAME})
endif()
target_link_directories(${LLBC_WRAP_LIB_PYLLBC}
                        PRIVATE "${LLBC_3RD_CPYTHON_LIB_OUTPUT_DIR}")
llbc_apply_config_postfix(${LLBC_WRAP_LIB_PYLLBC})
add_dependencies(${LLBC_WRAP_LIB_PYLLBC} pyllbc_prebuild)
set_target_properties(${LLBC_WRAP_LIB_PYLLBC} PROPERTIES
                      FOLDER "wrap"
                      SUFFIX ".pyd")