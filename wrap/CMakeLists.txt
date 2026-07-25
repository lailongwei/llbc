# Add pyllbc.
if (EXISTS "${LLBC_3RD_DIR_CPYTHON}/README")
    add_subdirectory(${LLBC_WRAP_PYLLBC_DIR})
else()
    message(NOTICE "3rd - cpython not found(dir: ${LLBC_3RD_DIR_CPYTHON}), pyllbc will be skipped")
    message(NOTICE "- run <git submodule update --init wrap/pyllbc/cpython> to update")
endif()

# Add lullbc.
# if (EXISTS "${LLBC_3RD_DIR_LUA}/lua.c")
#     add_subdirectory(${LLBC_WRAP_LULLBC_DIR})
# else()
#     message(NOTICE "3rd - lua not found(dir: ${LLBC_3RD_DIR_LLVM}), lullbc will be skipped")
#     message(NOTICE "- run <git submodule update --init wrap/lullbc/lua> to update")
# endif()

# Add csllbc.
# if (WIN32)
#     add_subdirectory({LLBC_WRAP_CSLLBC_DIR})
# else()
#     message(NOTICE "csllbc wrap target not supported for ${CMAKE_SYSTEM_NAME}")
# endif()