--  @file   premake5_com.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script common variables/functions define.

-- Capture shell cmd's output function define.
local function os_capture(cmd, raw)
    local f = assert(io.popen(cmd .. ' 2>&1', 'r'))
    local s = assert(f:read('*a'))
    f:close()
    if raw then return s end
    s = string.gsub(s, '^%s+', '')
    s = string.gsub(s, '%s+$', '')
    s = string.gsub(s, '[\n\r]+', ' ')
    return s
end

-- custom c/cpp toolset setting function define.
function set_custom_ccpp_toolset(toolset_bin_path)
    -- copy toolset default variables/functions from gcc/clang toolset.
    local custom_ccpp_toolset = {}
    local orig_gcc_tool = premake.tools.gcc -- or premake.tools.clang
    for k, v in pairs(orig_gcc_tool) do
        custom_ccpp_toolset[k] = v
    end

    -- override gettoolname() function.
    function custom_ccpp_toolset.gettoolname(cfg, tool)
        if tool == 'cc' then
            return toolset_bin_path .. '/gcc'
        elseif tool == 'cxx' then
            return toolset_bin_path .. '/g++'
        elseif tool == 'ar' then
            return toolset_bin_path .. '/ar'
        else
            return nil
        end
    end

    -- install to premake.
    premake.tools.custom_ccpp_toolset = custom_ccpp_toolset
end

-- llbc project fast include function.
function include_llbc_core_lib()
    -- includedirs.
    includedirs {
        llbc_core_lib_path .. "/include",
    }

    -- links.
    libdirs { llbc_output_dir }
    filter { "system:linux" }
        links {
            "dl",
			"pthread",
        }

    filter { "system:not windows", "configurations:debug*" }
        links {
            "llbc_debug",
        }

    filter { "system:not windows", "configurations:release*" }
        links {
            "llbc",
        }

    filter { "system:windows" }
        links {
            "ws2_32",
        }

    filter { "system:windows", "configurations:debug*" }
        links {
            "libllbc_debug",
        }

    filter { "system:windows", "configurations:release*" }
        links {
            "libllbc",
        }
    filter {}
end

-- determine system type.
llbc_system_types = {
    ['windows'] = 'windows',
    ['linux'] = 'linux',
    ['darwin'] = 'darwin',
    ['unknown'] = 'unknown',
}

llbc_system_type = llbc_system_types.unknown
if _ACTION:match('vs') then
    llbc_system_type = llbc_system_types.windows
else
    local sys_name = string.lower(os_capture('uname'))
    if sys_name:match('^linux') ~= nil then
        llbc_system_type = llbc_system_types.linux
    elseif sys_name:match('^darwin') ~= nil then
        llbc_system_type = llbc_system_types.darwin
    else
        llbc_system_type = llbc_system_types.unknown
    end
end

if llbc_system_type == llbc_system_types.unknown then
    error('Unsupported system, for now, llbc framework supported systems: windows/linux(and linux like)/darwin')
end

-- determine architecture type.
llbc_arch_types = {
    ['x86'] = 'x86',
    ['arm'] = 'ARM',
    ['unknown'] = 'unknown',
}

llbc_arch_type = llbc_arch_types.x86
if llbc_system_type ~= llbc_system_types.windows then
    local machine_arch = os_capture('uname -m'):lower()
    if machine_arch:sub(1, #'x86') == 'x86' then
        llbc_arch_type = llbc_arch_types.x86
    elseif machine_arch:sub(1, #'arm') == 'arm' then
        llbc_arch_type = llbc_arch_types.arm
    else
        llbc_arch_type = llbc_arch_types.unknown
    end
end
if llbc_arch_type == llbc_arch_types.unknown then
    error('Unsupported architecture, for now, llbc framework supported systems: x86/ARM')
end

llbc_arch_connect_char = '_'
if llbc_arch_type == llbc_arch_types.arm then
    llbc_arch_connect_char = ''
end

---- solution/projects path.
llbc_sln_path = "../.."
llbc_core_lib_path = llbc_sln_path .. "/llbc"
llbc_testsuite_path = llbc_sln_path .. "/testsuite"
llbc_quick_start_path = llbc_sln_path .. "/quick_start"
llbc_wraps_path = llbc_sln_path .. "/wrap"
llbc_py_wrap_path = llbc_wraps_path .. "/pyllbc"
llbc_lu_wrap_path = llbc_wraps_path .. "/lullbc"
llbc_cs_wrap_path = llbc_wraps_path .. "/csllbc"
llbc_lua_src_path = llbc_lu_wrap_path .. "/lua"

-- python exec path.
llbc_py_exec_path = nil
if llbc_system_type == llbc_system_types.windows then
    llbc_py_exec_path = "$(ProjectDir)../../tools/py.exe"
else
    local output = os_capture("python --version")
    if output:find("command not found") then
        output = os_capture("python3 --version")
        if output:find("command not found") then
            error("python command not found")
        else
            llbc_py_exec_path="python3"
        end
    else
        llbc_py_exec_path = "python"
    end
end

-- All libraries output directory.
llbc_output_dir = nil
local llbc_output_base_dir = llbc_sln_path .. "/output/" .. _ACTION
if llbc_system_type == llbc_system_types.windows then
    llbc_output_dir = llbc_output_base_dir .. "/$(Configuration)"
else
    llbc_output_dir = llbc_output_base_dir .. "/$(config)"
end

-- wrap libraries testsuite directory.
llbc_wrap_testsuite_output_dir = llbc_output_dir .. '/wrap_testsuites'

-- building script directory.
llbc_building_script_dir = llbc_sln_path .. "/tools/building_script"





