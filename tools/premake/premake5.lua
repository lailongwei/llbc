--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script define.

-- #########################################################################
-- Capture shell cmd's output function define.
local function os_capture(cmd, raw)
    local f = assert(io.popen(cmd, 'r'))
    local s = assert(f:read('*a'))
    f:close()
    if raw then return s end
    s = string.gsub(s, '^%s+', '')
    s = string.gsub(s, '%s+$', '')
    s = string.gsub(s, '[\n\r]+', ' ')
    return s
end

-- custom c/cpp toolset setting function define.
local function set_custom_ccpp_toolset(toolset_bin_path)
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

-- #########################################################################
-- Global compile settings.

-- cxx11 abi support flag.
local llbc_disable_cxx11_abi = true -- true/false.

-- set compile toolset.
local llbc_ccpp_compile_toolset = nil -- nil/''/gcc/clang/msc/custom_ccpp_toolset.

-- set custom compile toolset, if <llbc_ccpp_compile_toolset> set to 'custom_ccpp_toolset'.
-- set_custom_ccpp_toolset('<path to ccpp compiler toolset bin path>')

-- windows platform flag.
local llbc_is_windows_platform = string.match(_ACTION, 'vs') ~= nil

---- solution/projects path.
llbc_sln_path = "../.."
llbc_core_lib_path = llbc_sln_path .. "/llbc"
llbc_testsuite_path = llbc_sln_path .. "/testsuite"
llbc_wraps_path = llbc_sln_path .. "/wrap"
llbc_py_wrap_path = llbc_wraps_path .. "/pyllbc"
llbc_lu_wrap_path = llbc_wraps_path .. "/lullbc"
llbc_cs_wrap_path = llbc_wraps_path .. "/csllbc"

-- python exec path.
local llbc_py_exec_path
if llbc_is_windows_platform then
    llbc_py_exec_path = "$(ProjectDir)../../tools/py.exe"
else
    local output = os_capture("python --version")
    if output:find("command not found") then
        error("python command not found")
    else
        llbc_py_exec_path = "python"
    end
end

-- All libraries output directory.
local llbc_output_dir
local llbc_output_base_dir = llbc_sln_path .. "/output/" .. _ACTION
if llbc_is_windows_platform then
    llbc_output_dir = llbc_output_base_dir .. "/$(Configuration)"
else
    llbc_output_dir = llbc_output_base_dir .. "/$(config)"
end

-- wrap libraries testsuite directory.
local llbc_wrap_testsuite_output_dir = llbc_output_dir .. '/wrap_testsuites'

-- #########################################################################

workspace ("llbc_" .. _ACTION)
    -- location define.
    location (llbc_sln_path .. "/build/" .. _ACTION)
    -- target directory define.
    targetdir (llbc_output_dir)
    -- start project.
    startproject("testsuite")

    -- configurations.
    configurations { "release32", "debug32", "release64", "debug64" }

    -- architecture.
    filter { "configurations:*32" }
        architecture "x86"
    filter {}
    filter { "configurations:*64" }
        architecture "x86_64"
    filter {}

    -- not use cxx11 abi.
    if llbc_disable_cxx11_abi then
        filter { "language:c++", "system:not windows" }
        defines {
            "_GLIBCXX_USE_CXX11_ABI=0",
        }
        filter {}
    else -- enable cxx11 abi.
        filter { "language:c++", "system:not windows" }
        defines {
            "_GLIBCXX_USE_CXX11_ABI=1",
        }
        filter {}
    end

    -- defines.
    filter { "configurations:debug*" }
        defines {
            "DEBUG"
        }
    filter {}

    -- disable min/max macro define on windows platform.
    filter { "language:c++", "system:windows" }
        defines {
            "NOMINMAX"
        }
    filter {}

    -- enable symbols.
    symbols "On"

    -- set optimize options.
    filter { "configurations:debug*" }
        runtime "Debug"
        optimize "Debug"
    filter {}
    filter { "configurations:release*" }
        optimize "On"
    filter {}

    -- warnings setting.
    filter { "system:windows", "language:c++" }
        disablewarnings {
            "4091",
            "4068",
            "4251",
        }
    filter {}
    filter { "system:not windows", "language:c++" }
        buildoptions { 
            "-Wall -Werror",
            "-Wno-strict-aliasing",
        }
    filter {}

    -- set debug target suffix.
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile(only avabilable on windows).
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild" }
    filter {}

    -- characterset architecture.
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

    -- enable obtaining backtraces from within a program.
    filter { "system:not windows", "language:c++" }
        linkoptions { "-rdynamic" }
    filter {}

-- ****************************************************************************
-- llbc core library compile setting.
project "llbc"
    -- language, kind.
    language "c++"
    kind "SharedLib"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- files.
    files {
        llbc_sln_path .. "/CHANGELOG",
        llbc_sln_path .. "/README.md",

        llbc_core_lib_path .. "/**.h",
        llbc_core_lib_path .. "/**.c",
        llbc_core_lib_path .. "/**.cpp",

        llbc_sln_path .. "/tools/premake/*.lua",
    }

    filter { "system:macosx" }
        files {
            llbc_core_lib_path .. "/**.mm",
        }
    filter {}

    -- includedirs.
    includedirs {
        llbc_sln_path,
        llbc_core_lib_path .. "/include",
    }

    -- target prefix.
    targetprefix "lib"

    -- links.
    filter { "system:linux" }
        links {
            "dl",
            "uuid",
            "pthread",
        }
    filter { "system:windows" }
        links {
            "ws2_32",
            "Mswsock",
            "DbgHelp",
        }
    filter { "system:macosx" }
        links {
            "iconv",
        }
    filter {}

    -- Enable c++11 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++11",
        }
    filter {}

    -- Default hidden symbols.
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

-- ****************************************************************************
-- core library testsuite compile setting.
project "testsuite"
    -- language, kind.
    language "c++"
    kind "ConsoleApp"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- dependents.
    dependson {
        "llbc",
    }

    -- files.
    files {
        llbc_testsuite_path .. "/**.h",
        llbc_testsuite_path .. "/**.cpp",
        llbc_testsuite_path .. "/**.xml",
        llbc_testsuite_path .. "/**.cfg",
        llbc_testsuite_path .. "/**.ini",
    }

    -- includedirswrap\csllbc\csharp\script_tools.
    includedirs {
        llbc_core_lib_path .. "/include",
        llbc_testsuite_path,
    }

    -- links.
    libdirs { llbc_output_dir }
    filter { "system:linux" }
        links {
            "dl",
			"pthread",
        }
    filter {}

    filter { "system:not windows", "configurations:debug*" }
        links {
            "llbc_debug",
        }
    filter {}

    filter { "system:not windows", "configurations:release*" }
        links {
            "llbc",
        }
    filter {}

    filter { "system:windows" }
        links {
            "ws2_32",
        }
    filter {}

    filter { "system:windows", "configurations:debug*" }
        links {
            "libllbc_debug",
        }
    filter {}

    filter { "system:windows", "configurations:release*" }
        links {
            "libllbc",
        }
    filter {}

    -- warnings.
    -- filter { "system:not windows" }
    --     disablewarnings {
    --         "invalid-source-encoding",
    --     }
    -- filter {}

    -- Enable c++11 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++11",
        }
    filter {}

    -- Specific debug directory.
    debugdir(llbc_output_dir)

    -- Copy all testcases config files to output directory.
    postbuildmessage("Copying all testcase config files to output directory...");
    local test_cfgs = {
        "core/config/IniTestCfg.ini",
        "core/log/LogTestCfg.cfg",
        "core/log/LogTestCfg.xml",
        "app/AppCfgTest.cfg",
        "app/AppCfgTest.ini",
        "app/AppCfgTest.xml",
    }
    filter { "system:windows" }
        for _, test_cfg in pairs(test_cfgs) do
            postbuildcommands(string.format("COPY /Y \"%s\\%s\" \"%s\"",
                                            string.gsub(llbc_testsuite_path, "/", "\\"),
                                            string.gsub(test_cfg, "/", "\\"),
                                            string.gsub(llbc_output_dir, "/", "\\")))
        end
    filter {}
    filter { "system:not windows" }
        for _, test_cfg in pairs(test_cfgs) do
            postbuildcommands(string.format("cp -rf \"%s/%s\" \"%s\"", llbc_testsuite_path, test_cfg, llbc_output_dir))
        end
    filter {}

group "wrap"

-- ****************************************************************************
-- python wrap library(pyllbc) compile setting.
-- import pylib_setting.
package.path = package.path .. ";" .. "../../wrap/pyllbc/?.lua"
local llbc_pylib_setting = require "pylib_setting"
local pylib_py_ver = llbc_pylib_setting.py_ver or 2
if pylib_py_ver == 2 then
elseif pylib_py_ver == 3 then
else
    error('Please set the python version number correctly(in wrap/pyllbc/pylib_setting.lua)!')
end

local llbc_pylib_incl_path = (llbc_pylib_setting.py_path[1] ~= nil and llbc_pylib_setting.py_path[1] ~= "") and llbc_pylib_setting.py_path[1] or ""
local llbc_pylib_lib_dir = (llbc_pylib_setting.py_path[2] ~= nil and llbc_pylib_setting.py_path[2] ~= "") and llbc_pylib_setting.py_path[2] or ""
local pyllbc_pylib_lib_name = (llbc_pylib_setting.py_path[3] ~= nil and llbc_pylib_setting.py_path[3] ~= "") and llbc_pylib_setting.py_path[3] or ""
project "pyllbc"
    -- language, kind.
    language "c++"
    kind "SharedLib"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- dependents.
    dependson {
        "llbc",
    }

    -- files.
    files {
        llbc_py_wrap_path .. "/include/**.h",
        llbc_py_wrap_path .. "/src/**.h",
        llbc_py_wrap_path .. "/src/**.cpp",

        llbc_py_wrap_path .. "/script/**.py",

        llbc_py_wrap_path .. "/testsuite/**.py",
    }

    if pylib_py_ver == 2 then
        files {
            llbc_py_wrap_path .. "/Python2.7.8/**.h" ,
        }
    elseif pylib_py_ver == 3 then
        files {
            llbc_py_wrap_path .. "/Python3.8.5/**.h",
        }
    end

    -- includedirs.
    includedirs {
        llbc_core_lib_path .. "/include",

        llbc_py_wrap_path .. "/include",
        llbc_py_wrap_path,
    }

    if string.len(llbc_pylib_incl_path) > 0 then
        includedirs { llbc_pylib_incl_path }
    else -- if not specific python include path, windows platform will use specific version python, other platforms will auto detect.
        filter { "system:windows" }
            if pylib_py_ver == 2 then
                includedirs { llbc_py_wrap_path .. "/Python2.7.8/Include" }
            elseif pylib_py_ver == 3 then
                includedirs { llbc_py_wrap_path .. "/Python3.8.5/Include" }
            end
        filter {}

        filter { "system:not windows" }
            if pylib_py_ver == 2 then
                includedirs { "/usr/include/python2.7" }
            elseif pylib_py_ver == 3 then
                includedirs { "/usr/include/python3.8" }
            end
        filter {}
    end

    -- define HAVE_ROUND(only on vs2013, vs2015, vs2017 and later version visual studio IDEs).
    filter { "action:vs20*" }
        defines { "HAVE_ROUND" }
    filter {}

    -- prebuild: Export native method export code & integrate python script code(to c++ dynamic lib).
    prebuildcommands {
        llbc_py_exec_path .. " ../../tools/building_script/py_prebuild.py pyllbc",
    }

    -- postbuild: Create testsuite output directory.
    local pyllbc_testsuite_path = llbc_py_wrap_path .. "/testsuite"
    local pyllbc_testsuite_path_win = string.gsub(pyllbc_testsuite_path, "/", "\\")
    local pyllbc_testsuite_output_path = llbc_wrap_testsuite_output_dir .. '/pyllbc'
    local pyllbc_testsuite_output_path_win = string.gsub(pyllbc_testsuite_output_path, "/", "\\")
    filter { "system:windows" }
        postbuildcommands(
            string.format(
                "IF exist \"%s\" (echo \"%s exist\") ELSE (echo \"Create testsuite output dir: %s\" && MD \"%s\")",
                pyllbc_testsuite_output_path_win, pyllbc_testsuite_output_path_win,
                pyllbc_testsuite_output_path_win, pyllbc_testsuite_output_path_win))
    filter { "system:not windows" }
        postbuildcommands(string.format("mkdir -p \"%s\"", pyllbc_testsuite_output_path))
    filter {}

    -- postbuild: Copy testsuite scripts to output directory.
    filter { "system:windows" }
        postbuildcommands(
            string.format("XCOPY /Y /E /F \"%s\\*\" \"%s\"",
                pyllbc_testsuite_path_win,
                pyllbc_testsuite_output_path_win))
    filter { "system:not windows" }
        postbuildcommands(string.format("\\cp -rf \"%s\"/* \"%s\"",
            pyllbc_testsuite_path, pyllbc_testsuite_output_path))
    filter {}

    -- postbuild: Copy core lib & pyllbc lib file.
    local llbc_output_dir_win = string.gsub(llbc_output_dir, "/", "\\")
    filter { "system:windows" }
        postbuildcommands(
            string.format(
                "for /f \"tokens=*\" %%%%f in ('DIR /b /a-d \"%s\\libllbc*.dll\"') do (COPY /Y %s\\%%%%f \"%s\")",
                llbc_output_dir_win, llbc_output_dir_win, pyllbc_testsuite_output_path_win))
        postbuildcommands(
            string.format(
                "for /f \"tokens=*\" %%%%f in ('DIR /b /a-d \"%s\\llbc*.pyd\"') do (COPY /Y %s\\%%%%f \"%s\")",
                string.gsub(llbc_output_dir, "/", "\\"),
                string.gsub(llbc_output_dir, "/", "\\"),
                pyllbc_testsuite_output_path_win))
    filter { "system:not windows" }
        -- Do nothing.
    filter {}

    -- postbuild: Copy misc files.
    local pyllbc_need_copy_to_testsuite_ouptut_misc_files = {
        llbc_testsuite_path .. "/core/log/LogTestCfg.cfg"
    }
    filter { "system:windows" }
        for _, file in pairs(pyllbc_need_copy_to_testsuite_ouptut_misc_files) do
            postbuildcommands(string.format("COPY /Y \"%s\" \"%s\"",
                                            string.gsub(file, "/", "\\"), pyllbc_testsuite_output_path_win))
        end
    filter {}
    filter { "system:not windows" }
        for _, file in pairs(pyllbc_need_copy_to_testsuite_ouptut_misc_files) do
            postbuildcommands(string.format("\\cp -rf \"%s\" \"%s\"", file, pyllbc_testsuite_output_path))
        end
    filter {}

    -- target name, target prefix, extension.
    targetname "llbc"
    targetprefix ""
    filter { "system:windows" }
        targetextension ".pyd"
    filter {}

    -- links.
    -- link llbc library.
    libdirs { llbc_output_dir }

    filter { "system:windows", "configurations:debug*" }
        links { "libllbc_debug" }
    filter {}
    filter { "system:windows", "configurations:release*" }
        links { "libllbc" }
    filter {}

    filter { "system:not windows", "configurations:debug*" }
        links { "llbc_debug" }
    filter {}
    filter { "system:not windows", "configurations:release*" }
        links { "llbc" }
    filter {}

    -- link python library.
    if string.len(llbc_pylib_lib_dir) > 0 then
        libdirs { llbc_pylib_lib_dir }
    else
        filter { "system:windows", "architecture:x86" }
            if pylib_py_ver == 2 then
                libdirs { llbc_py_wrap_path .. "/Python2.7.8/Libs/Win/32" }
            elseif pylib_py_ver == 3 then
                libdirs { llbc_py_wrap_path .. "/Python3.8.5/Libs/Win/32" }
            end
        filter {}
        filter { "system:windows", "architecture:x64" }
            if pylib_py_ver == 2 then
                libdirs { llbc_py_wrap_path .. "/Python2.7.8/Libs/Win/64" }
            elseif pylib_py_ver == 3 then
                libdirs { llbc_py_wrap_path .. "/Python3.8.5/Libs/Win/64" }
            end
        filter {}
    end

    if string.len(pyllbc_pylib_lib_name) > 0 then
        links { pyllbc_pylib_lib_name }
    else
        -- in windows platform, python library use the python library file in the repo.
        filter { "system:windows", "configurations:debug*" }
            if pylib_py_ver == 2 then
                links { "python27_d" }
            elseif pylib_py_ver == 3 then
                links { "python38_d" }
            end
        filter {}
        filter { "system:windows", "configurations:release*" }
            if pylib_py_ver == 2 then
                links { "python27" }
            elseif pylib_py_ver == 3 then
                links { "python38" }
            end
        filter {}

        -- in non-windows platform, python library default named: python2.7.
        filter { "system:not windows" }
            if pylib_py_ver == 2 then
                links { "python2.7" }
            elseif pylib_py_ver == 3 then
                links { "python3.8" }
            end
        filter {}
    end

    -- Enable c++11 support.
    filter { "system:not windows", "language:c++" }
        buildoptions {
            "-std=c++11"
        }
    filter {}

    -- Default hidden symbols.
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

group "wrap/csllbc"

-- ****************************************************************************
-- csharp wrap library(csllbc) native library compile setting.
project "csllbc_native"
    -- language, kind.
    language "c++"
    kind "SharedLib"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- dependents.
    dependson {
        "llbc",
    }

    -- library suffix.
    targetprefix "lib"

    -- files.
    files {
        llbc_cs_wrap_path .. "/native/**.h",
        llbc_cs_wrap_path .. "/native/**.cpp",
    }

    -- includedirs.
    includedirs {
        llbc_core_lib_path .. "/include",
        llbc_cs_wrap_path .. "/native/include",
    }

    -- links.
    libdirs {
        llbc_output_dir,
    }
    filter { "system:windows", "configurations:debug*" }
        links {
            "libllbc_debug",
        }
    filter {}
    filter { "system:windows", "configurations:release*" }
        links {
            "libllbc",
        }
    filter {}
    filter { "system:not windows", "configurations:debug*" }
        links {
            "llbc_debug",
        }
    filter {}
    filter { "system:not windows", "configurations:release*" }
        links {
            "llbc",
        }
    filter {}

    -- disable warnings.
    filter { "system:not windows" }
        disablewarnings {
            "attributes"
        }
    filter {}

    -- Enable c++11 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++11",
        }
    filter {}

    -- Default hidden symbols.
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}


-- ****************************************************************************
-- csharp wrap library(csllbc) compile setting.
project "csllbc"
    -- language, kind.
    kind "SharedLib"
    language "c#"

    -- files.
    files {
        llbc_cs_wrap_path .. "/csharp/**.cs",
    }

    -- dependents.
    dependson {
        "llbc",
        "csllbc_native",
    }

    -- set unsafe flag.
    clr "Unsafe"

    -- prebuild commands.
    filter { "system:windows" }
        prebuildcommands {
            llbc_py_exec_path .. ' -c "import os;print(os.getcwd())"',
            llbc_py_exec_path .. " ../../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
            llbc_py_exec_path .. " ../../../wrap/csllbc/csharp/script_tools/gen_errno_code.py",
        }
    filter {}
    filter { "system:not windows" }
        prebuildcommands {
            llbc_py_exec_path .. ' -c "import os;print(os.getcwd())"',
            llbc_py_exec_path .. " ../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
            llbc_py_exec_path .. " ../../wrap/csllbc/csharp/script_tools/gen_errno_code.py",
        }
    filter {}

    -- postbuild commands.
    filter { "system:not windows" }
        postbuildcommands {
            llbc_py_exec_path .. " ../../wrap/csllbc/csharp/script_tools/gen_dll_cfg.py ../../output/" .. _ACTION,
        }
    filter {}

    -- defines.
    filter { "system:linux" }
        defines {
            "CSLLBC_TARGET_PLATFORM_LINUX",
        }
    filter {}
    filter { "system:windows" }
        defines {
            "CSLLBC_TARGET_PLATFORM_WIN32",
        }
    filter {}

    -- links.
    filter {}
    links {
        "System",
        "System.Net",
        "System.Core",
    }

-- ****************************************************************************
-- csharp wrap library(csllbc) testsuite compile setting.
project "csllbc_testsuite"
    -- language, kind.
    kind "ConsoleApp"
    language "c#"

    -- dependents.
    dependson {
        "llbc",
        "csllbc_native",
        "csllbc",
    }

    -- files.
    files {
        llbc_cs_wrap_path .. "/testsuite/**.cs",
    }

    -- links.
    links {
        "System",
        "System.Net",
        "System.Core",
        "csllbc",
    }

group "wrap/lullbc"

-- ****************************************************************************
-- luasrc library(liblua) compile setting.
local llbc_lua_src_path = "../../wrap/lullbc/lua"
project "lullbc_lualib"
    -- language, kind.
    language "c++"
    kind "SharedLib"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- files.
    files {
        llbc_lua_src_path .. "/*.h",
        llbc_lua_src_path .. "/*.c",
    }
    removefiles {
        llbc_lua_src_path .. "/lua.c",
        llbc_lua_src_path .. "/luac.c",
        llbc_lua_src_path .. "/onelua.c",
    }

    -- defines.
    defines {
        "LUA_COMPAT_5_1",
        "LUA_COMPAT_5_2",
        "LUA_COMPAT_5_3",
    }
    filter { "system:windows" }
        defines { "LUA_BUILD_AS_DLL" }
    filter {}
    filter { "system:not windows" }
        defines { "LUA_USE_DLOPEN" }
    filter {}

    -- links.
    filter { "system:not windows" }
        links { "dl" }
    filter {}

    -- target name, target prefix.
    targetname "lua"
    targetprefix "lib"

-- lua executable compile setting.
local llbc_lua_src_path = llbc_lu_wrap_path .. "/lua"
project "lullbc_luaexec"
    -- language, kind.
    language "c++"
    kind "ConsoleApp"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- files.
    files {
        llbc_lua_src_path .. "/*.h",
        llbc_lua_src_path .. "/lua.c",
    }

    -- defines.
    defines {
        "LUA_COMPAT_5_1",
        "LUA_COMPAT_5_2",
        "LUA_COMPAT_5_3",
    }

    -- dependents.
    dependson {
        "lullbc_lualib"
    }

    -- links.
    libdirs { 
        llbc_output_dir,
    }

    filter { "system:not windows" }
        links { "dl" }
    filter {}

    filter { "configurations:debug*", "system:windows" }
        links { "liblua_debug" }
    filter {}
    filter { "configurations:release*", "system:windows" }
        links { "liblua" }
    filter {}
    filter { "configurations:debug*", "system:not windows" }
        links { "lua_debug" }
    filter {}
    filter { "configurations:release*", "system:not windows" }
        links { "lua" }
    filter {}
 
    -- target name, target prefix.
    targetname "lua"

-- lua wrap library(lullbc) compile setting.
-- import lualib_setting.
package.path = package.path .. ";" .. "../../wrap/lullbc/?.lua"
local llbc_lualib_setting = require "lualib_setting"
local llbc_lualib_incl_path = (llbc_lualib_setting.lua_path[1] ~= nil and llbc_lualib_setting.lua_path[1] ~= "") and llbc_lualib_setting.lua_path[1] or llbc_lua_src_path
local llbc_lualib_lib_dir = (llbc_lualib_setting.lua_path[2] ~= nil and llbc_lualib_setting.lua_path[2] ~= "") and llbc_lualib_setting.lua_path[2] or llbc_output_dir
project "lullbc"
    -- language, kind.
    language "c++"
    kind "SharedLib"

    -- toolset.
    if llbc_ccpp_compile_toolset ~= nil and llbc_ccpp_compile_toolset ~= '' then
        toolset(llbc_ccpp_compile_toolset)
    end

    -- dependents.
    dependson {
        "llbc",
        "lullbc_lualib",
        "lullbc_luaexec",
    }

    -- files.
    files {
        llbc_lu_wrap_path .. "/lua/*.h",

        llbc_lu_wrap_path .. "/include/**.h",
        llbc_lu_wrap_path .. "/src/**.h",
        llbc_lu_wrap_path .. "/src/**.c",
        llbc_lu_wrap_path .. "/src/**.cpp",

        llbc_lu_wrap_path .. "/script/**.lua",

        llbc_lu_wrap_path .. "/testsuite/**.lua",
        llbc_lu_wrap_path .. "/testsuite/**.cfg",
    }

    -- define targetextension.
    filter { "system:macosx" }
        targetextension ".so"
    filter {}

    -- includedirs.
    includedirs {
        llbc_lualib_incl_path,
        llbc_core_lib_path .. "/include",
        llbc_lu_wrap_path .. "/include",
        llbc_lu_wrap_path,
    }

    -- defines.
    filter { "system:windows", "action:vs2013 and vs2015 and vs2017" }
    defines {
        "HAVE_ROUND",
    }
    filter {}

    -- prebuild commands.
    filter { "configurations:debug*" }
        prebuildcommands {
            llbc_py_exec_path .. " ../../tools/building_script/lu_prebuild.py lullbc debug",
        }
        postbuildcommands {
            llbc_py_exec_path .. string.format(' ../../tools/building_script/lu_postbuild.py %s %s "%s"', "lullbc", "debug", llbc_output_dir),
        }
    filter {}

    filter { "configurations:release*" }
        prebuildcommands {
            llbc_py_exec_path .. " ../../tools/building_script/lu_prebuild.py lullbc release",
        }
        postbuildcommands {
            llbc_py_exec_path .. string.format(' ../../tools/building_script/lu_postbuild.py %s %s "%s"', "lullbc", "release", llbc_output_dir),
        }
    filter {}

    -- target name, target prefix, extension.
    targetname "_lullbc"
    targetprefix ""

    -- links.
    libdirs { 
        llbc_output_dir,
        llbc_lualib_lib_dir,
    }

    filter { "configurations:debug*", "system:windows" }
        links {
            "libllbc_debug",
            (llbc_lualib_setting.lua_path[3] ~= nil and llbc_lualib_setting.lua_path[3] ~= "") and llbc_lualib_setting.lua_path[3] or "liblua_debug",
        }
    filter {}
    filter { "configurations:debug*", "system:linux" }
        links {
            "llbc_debug",
            (llbc_lualib_setting.lua_path[3] ~= nil and llbc_lualib_setting.lua_path[3] ~= "") and llbc_lualib_setting.lua_path[3] or "lua_debug",
        }
    filter {}
    filter { "configurations:debug*", "system:macosx" }
        links {
            "llbc_debug",
        }
    filter {}

    filter { "configurations:release*", "system:windows" }
        links {
            "libllbc",
            (llbc_lualib_setting.lua_path[3] ~= nil and llbc_lualib_setting.lua_path[3] ~= "") and llbc_lualib_setting.lua_path[3] or "liblua",
        }
    filter {}
    filter { "configurations:release*", "system:linux" }
        links {
            "llbc",
            (llbc_lualib_setting.lua_path[3] ~= nil and llbc_lualib_setting.lua_path[3] ~= "") and llbc_lualib_setting.lua_path[3] or "lua",
        }
    filter {}
    filter { "configurations:release*", "system:macosx" }
        links {
            "llbc",
        }
    filter {}

    -- Enable c++11 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++11",
        }
    filter {}

    -- Default hidden symbols.
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

    -- linkoptions.
    filter { "system:macosx" }
    	linkoptions {
            "-undefined dynamic_lookup",
        }
    filter {}

group ""
