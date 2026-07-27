# Add example.
add_subdirectory(${LLBC_LIB_EXAMPLE_DIR})

#  Add function test.
add_subdirectory(${LLBC_LIB_FUNC_TEST_DIR})

# Add unit test.
if (EXISTS "${LLBC_3RD_DIR_GOOGLETEST}/CMakeLists.txt")
	add_subdirectory(${LLBC_3RD_DIR_GOOGLETEST} ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
	set_target_properties(gtest PROPERTIES FOLDER "tests/3rdparty")
	add_subdirectory(${LLBC_LIB_UNIT_TEST_DIR})
else()
	message(NOTICE "3rd - googletest not found(dir: ${LLBC_3RD_DIR_GOOGLETEST}), unit_test will be skipped")
	message(NOTICE "- run <git submodule update --init tests/3rdparty/googletest> to update")
endif()
