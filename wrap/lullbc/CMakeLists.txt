# Collect lua lib sources.
file(GLOB LUA_LIB_SOURCES "${LLBC_3RD_DIR_LUA}/*.h" "${LLBC_3RD_DIR_LUA}/*.c")
list(REMOVE_ITEM LUA_LIB_SOURCES
     "${LLBC_3RD_DIR_LUA}/lua.c"
     "${LLBC_3RD_DIR_LUA}/luac.c"
     "${LLBC_3RD_DIR_LUA}/onelua.c")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/lua FILES ${LUA_LIB_SOURCES})

# Collect lib exec sources.
set(LUA_EXEC_SOURCES "${LLBC_3RD_DIR_LUA}/lua.c")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/lua FILES ${LUA_EXEC_SOURCES})

# Collect lullbc lib sources.
file(GLOB_RECURSE LULLBC_SOURCES include/*.h src/*.h src/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LULLBC_SOURCES})

# Build lua lib library(shared).
set(LLBC_3RD_LUA_LIB lullbc_lualib)
add_library(${LLBC_3RD_LUA_LIB} SHARED ${LUA_LIB_SOURCES})
target_include_directories(${LLBC_3RD_LUA_LIB}
                           PUBLIC "${LLBC_3RD_DIR_LUA}")
target_compile_definitions(${LLBC_3RD_LUA_LIB}
                           PUBLIC LUA_COMPAT_5_1 
                           PUBLIC LUA_COMPAT_5_2 
                           PUBLIC LUA_COMPAT_5_3
                           PRIVATE "$<IF:$<PLATFORM_ID:Windows>,LUA_BUILD_AS_DLL,LUA_USE_DLOPEN>")
target_link_libraries(${LLBC_3RD_LUA_LIB}
                      PRIVATE llbc_build_settings
                      PRIVATE "$<$<NOT:$<PLATFORM_ID:Windows>>:dl>"
                      PRIVATE "$<$<NOT:$<PLATFORM_ID:Windows>>:m>")
set_target_properties(${LLBC_3RD_LUA_LIB} PROPERTIES
                      FOLDER "wrap/lullbc"
                      OUTPUT_NAME "lua")
set_target_properties(${LLBC_3RD_LUA_LIB} PROPERTIES
                      C_VISIBILITY_PRESET default
                      CXX_VISIBILITY_PRESET default
                      VISIBILITY_INLINES_HIDDEN OFF)
if (WIN32)
    set_target_properties(${LLBC_3RD_LUA_LIB} PROPERTIES
                          PREFIX "lib"
                          ARCHIVE_OUTPUT_NAME "liblua")
endif()

# Add pre-build target: execute lullbc_prebuild.py script.
add_custom_target(lullbc_prebuild
                  COMMAND ${Python_EXECUTABLE}
                    "${LLBC_TOOLS_DIR}/build_scripts/lullbc_prebuild.py"
                    ${LLBC_VERSION}
                    ${LLBC_PLATFORM_ARCH} 
                    $<CONFIG>
                    ${LLBC_DISABLE_CXX11_ABI}
                    ${CMAKE_VS_DEVENV_COMMAND}
                  COMMENT "Pre-build ${LLBC_WRAP_LIB_LULLBC}")
set_target_properties(lullbc_prebuild PROPERTIES
                      FOLDER "_llbc_internal_targets")

# Build lua executable(link shared lua lib).
set(LLBC_3RD_LUA_EXEC lullbc_luaexec)
add_executable(${LLBC_3RD_LUA_EXEC} ${LUA_EXEC_SOURCES})
target_link_libraries(${LLBC_3RD_LUA_EXEC}
                      PRIVATE llbc_build_settings
                      PRIVATE ${LLBC_3RD_LUA_LIB})
set_target_properties(${LLBC_3RD_LUA_EXEC} PROPERTIES
                      FOLDER "wrap/lullbc"
                      OUTPUT_NAME "lua")

# Build lullbc library.
set(LLBC_WRAP_LIB_LULLBC lullbc)
add_library(${LLBC_WRAP_LIB_LULLBC} SHARED ${LULLBC_SOURCES})
add_dependencies(${LLBC_WRAP_LIB_LULLBC} lullbc_prebuild)
target_include_directories(${LLBC_WRAP_LIB_LULLBC}
                           PRIVATE ${LLBC_WRAP_DIR_LULLBC}/include
                           PRIVATE ${LLBC_WRAP_DIR_LULLBC})
target_link_libraries(${LLBC_WRAP_LIB_LULLBC}
                      PRIVATE llbc_build_settings ${LLBC_3RD_LUA_LIB} ${LLBC_LIB_SHARED})
add_dependencies(${LLBC_WRAP_LIB_LULLBC} ${LLBC_3RD_LUA_EXEC})
set_target_properties(${LLBC_WRAP_LIB_LULLBC} PROPERTIES
                      FOLDER "wrap/lullbc"
                      PREFIX ""
                      OUTPUT_NAME "_lullbc")
if (APPLE)
    set_target_properties(${LLBC_WRAP_LIB_LULLBC} PROPERTIES
                          SUFFIX ".so")
endif()

# Set targets postfix.
llbc_apply_config_postfix(${LLBC_3RD_LUA_LIB} ${LLBC_3RD_LUA_EXEC} ${LLBC_WARP_LIB_LULLBC})

# Add post-build command: execute lullbc_postbuild.py script.
add_custom_command(TARGET ${LLBC_WRAP_LIB_LULLBC}
                   POST_BUILD
                   COMMAND ${Python_EXECUTABLE}
                        "${LLBC_TOOLS_DIR}/build_scripts/lullbc_postbuild.py"
                        ${LLBC_VERSION}
                        ${LLBC_PLATFORM_ARCH} 
                        $<CONFIG>
                        ${LLBC_DISABLE_CXX11_ABI}
                        ${CMAKE_VS_DEVENV_COMMAND}
                   COMMENT "Post-build ${LLBC_WRAP_LIB_LULLBC}")



