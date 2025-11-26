--  @file   premake5_cfg.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake config file.

require "premake5_com"

-- disable/enable cxx11 abi support.
llbc_disable_cxx11_abi = true -- true/false.

-- enable/disable asan support.
-- Note: This config option only available in non-windows platform and c/c++ projects.
llbc_enable_asan_support = false

-- c/c++ compile toolset setting.
llbc_ccpp_compile_toolset = nil -- nil/''/gcc/clang/msc/custom_ccpp_toolset.

-- if <llbc_ccpp_compile_toolset> is 'custom_ccpp_toolset', call <set_custom_ccpp_toolset()> path.
-- set_custom_ccpp_toolset('path/to/your/custom/ccpp_toolset/bin/dir')
