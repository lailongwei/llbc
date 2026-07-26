# Recurse all tools script files.
file(GLOB_RECURSE TOOLS_FILES build_scripts/*.py cmake/config.cmake)

# Group tools files.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TOOLS_FILES})

# Define custom target for eidt in VS/xcode.
add_custom_target(llbc_tools SOURCES ${TOOLS_FILES})