# Add pyllbc.
if (EXISTS "${LLBC_3RD_DIR_CPYTHON}/README")
    add_subdirectory(${LLBC_WRAP_DIR_PYLLBC})
else()
    message(NOTICE "3rd - cpython not found(dir: ${LLBC_3RD_DIR_CPYTHON}), pyllbc will be skipped")
    message(NOTICE "- run <git submodule update --init wrap/pyllbc/cpython> to update")
endif()

# Add lullbc.
if (EXISTS "${LLBC_3RD_DIR_LUA}/lua.c")
    add_subdirectory(${LLBC_WRAP_DIR_LULLBC})
else()
    message(NOTICE "3rd - lua not found(dir: ${LLBC_3RD_DIR_LLVM}), lullbc will be skipped")
    message(NOTICE "- run <git submodule update --init wrap/lullbc/lua> to update")
endif()

# Add csllbc.
if (CMAKE_CSharp_COMPILER)
    add_subdirectory(${LLBC_WRAP_DIR_CSLLBC})
else()
    message(NOTICE "CSharp compiler not found, csllbc will be skipped")
endif()