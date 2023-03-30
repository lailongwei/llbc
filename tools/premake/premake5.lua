--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script define.

-- #########################################################################
-- Global compile settings

-- flags.
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil

-- solution/projects path
SLN_PATH = "../.."
CORELIB_PATH = SLN_PATH .. "/llbc"
TESTSUITE_PATH = SLN_PATH .. "/testsuite"
WRAPS_PATH = SLN_PATH .. "/wrap"
PY_WRAP_PATH = WRAPS_PATH .. "/pyllbc"
LU_WRAP_PATH = WRAPS_PATH .. "/lullbc"
CS_WRAP_PATH = WRAPS_PATH .. "/csllbc"

-- Capture shell cmd's output
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

-- PY target
local PY
if IS_WINDOWS then
    PY = "$(ProjectDir)../../tools/py.exe"
else
    local output = os_capture("python --version")
    if output:find("command not found") then
        error("python command not found")
    elseif output:find("Python 3") then
        if os_capture("which python2"):len() == 0 then
            error("Python3 is not yet supported, please install python2 and make sure python2 can be use")
        end
        PY = "python2"
    else
        PY = "python"
    end
end

-- All libraries output directory
LLBC_OUTPUT_BASE_DIR = SLN_PATH .. "/output/" .. _ACTION
if IS_WINDOWS then
    LLBC_OUTPUT_DIR = LLBC_OUTPUT_BASE_DIR .. "/$(Configuration)"
else
    LLBC_OUTPUT_DIR = LLBC_OUTPUT_BASE_DIR .. "/$(config)"
end

-- #########################################################################

workspace ("llbc_" .. _ACTION)
    -- location define
    location (SLN_PATH .. "/build/" .. _ACTION)
    -- target directory define
    targetdir (LLBC_OUTPUT_DIR)
    -- start project
    startproject("testsuite")

    -- configurations
    configurations { "release32", "debug32", "release64", "debug64" }

    -- architecture
    filter { "configurations:*32" }
        architecture "x86"
    filter {}
    filter { "configurations:*64" }
        architecture "x86_64"
    filter {}

    -- defines
    filter { "configurations:debug*" }
        defines {
            "DEBUG"
        }
    filter {}

    -- enable symbols
    symbols "On"

    -- set optimize options
    filter { "configurations:debug*" }
        runtime "Debug"
        optimize "Debug"
    filter {}
    filter { "configurations:release*" }
        optimize "On"
    filter {}

    -- warnings setting
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

    -- set debug target suffix
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile(only avabilable on windows)
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild" }
    filter {}

    -- characterset architecture
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

    -- enable obtaining backtraces from within a program
    filter { "system:not windows", "language:c++" }
        linkoptions { "-rdynamic" }
    filter {}

-- ****************************************************************************
-- llbc core library compile setting
project "llbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- files
    files {
        SLN_PATH .. "/CHANGELOG",
        SLN_PATH .. "/README.md",

        CORELIB_PATH .. "/**.h",
        CORELIB_PATH .. "/**.c",
        CORELIB_PATH .. "/**.cpp",

        SLN_PATH .. "/tools/premake/*.lua",
    }

    filter { "system:macosx" }
        files {
            CORELIB_PATH .. "/**.mm",
        }
    filter {}

    -- includedirs
    includedirs {
        SLN_PATH,
        CORELIB_PATH .. "/include",
    }

    -- target prefix
    targetprefix "lib"

    -- links
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
-- core library testsuite compile setting
project "testsuite"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

    -- dependents
    dependson {
        "llbc",
    }

    -- files
    files {
        TESTSUITE_PATH .. "/**.h",
        TESTSUITE_PATH .. "/**.cpp",
        TESTSUITE_PATH .. "/**.xml",
        TESTSUITE_PATH .. "/**.cfg",
        TESTSUITE_PATH .. "/**.ini",
    }

    -- includedirswrap\csllbc\csharp\script_tools
    includedirs {
        CORELIB_PATH .. "/include",
        TESTSUITE_PATH,
    }

    -- links
    libdirs { LLBC_OUTPUT_DIR }
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

    -- warnings
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
    debugdir(LLBC_OUTPUT_DIR)

    -- Copy all testcases config files to output directory.
    postbuildmessage("Copying all testcase config files to output directory...");
    local test_cfgs = {
        "core/config/IniTestCfg.ini",
        "core/log/LogTestCfg.cfg",
        "app/AppCfgTest.cfg",
        "app/AppCfgTest.ini",
        "app/AppCfgTest.xml",
    }
    filter { "system:windows" }
        for _, test_cfg in pairs(test_cfgs) do
            postbuildcommands(string.format("COPY /Y \"%s\\%s\" \"%s\"",
                                            string.gsub(TESTSUITE_PATH, "/", "\\"),
                                            string.gsub(test_cfg, "/", "\\"),
                                            string.gsub(LLBC_OUTPUT_DIR, "/", "\\")))
        end
    filter {}
    filter { "system:not windows" }
        for _, test_cfg in pairs(test_cfgs) do
            postbuildcommands(string.format("cp -rf \"%s/%s\" \"%s\"", TESTSUITE_PATH, test_cfg, LLBC_OUTPUT_DIR))
        end
    filter {}

group "wrap"

-- ****************************************************************************
-- python wrap library(pyllbc) compile setting
-- import pylib_setting
package.path = package.path .. ";" .. "../../wrap/pyllbc/?.lua"
local PYLIB_SETTING = require "pylib_setting"
local PYLIB_PY_VER = PYLIB_SETTING.py_ver or 2
if PYLIB_PY_VER == 2 then
elseif PYLIB_PY_VER == 3 then
else
    error('Please set the python version number correctly(in wrap/pyllbc/pylib_setting.lua)!')
end

local PYLIB_INCL_PATH = (PYLIB_SETTING.py_path[1] ~= nil and PYLIB_SETTING.py_path[1] ~= "") and PYLIB_SETTING.py_path[1] or ""
local PYLIB_LIB_DIR = (PYLIB_SETTING.py_path[2] ~= nil and PYLIB_SETTING.py_path[2] ~= "") and PYLIB_SETTING.py_path[2] or ""
local PYLIB_LIB_NAME = (PYLIB_SETTING.py_path[3] ~= nil and PYLIB_SETTING.py_path[3] ~= "") and PYLIB_SETTING.py_path[3] or ""
project "pyllbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- dependents
    dependson {
        "llbc",
    }

    -- files
    files {
        PY_WRAP_PATH .. "/include/**.h",
        PY_WRAP_PATH .. "/src/**.h",
        PY_WRAP_PATH .. "/src/**.cpp",

        PY_WRAP_PATH .. "/script/**.py",

        PY_WRAP_PATH .. "/testsuite/**.py",
    }

    if PYLIB_PY_VER == 2 then
        files {
            PY_WRAP_PATH .. "/Python2.7.8/**.h" ,
        }
    elseif PYLIB_PY_VER == 3 then
        files {
            PY_WRAP_PATH .. "/Python3.8.5/**.h",
        }
    end

    -- includedirs
    includedirs {
        CORELIB_PATH .. "/include",

        PY_WRAP_PATH .. "/include",
        PY_WRAP_PATH,
    }

    if string.len(PYLIB_INCL_PATH) > 0 then
        includedirs { PYLIB_INCL_PATH }
    else -- if not specific python include path, windows platform will use specific version python, other platforms will auto detect.
        filter { "system:windows" }
            if PYLIB_PY_VER == 2 then
                includedirs { PY_WRAP_PATH .. "/Python2.7.8/Include" }
            elseif PYLIB_PY_VER == 3 then
                includedirs { PY_WRAP_PATH .. "/Python3.8.5/Include" }
            end
        filter {}

        filter { "system:not windows" }
            if PYLIB_PY_VER == 2 then
                includedirs { "/usr/include/python2.7" }
            elseif PYLIB_PY_VER == 3 then
                includedirs { "/usr/include/python3.8" }
            end
        filter {}
    end

    -- define HAVE_ROUND(only on vs2013, vs2015, vs2017 and later version visual studio IDEs).
    filter { "action:vs20*" }
        defines { "HAVE_ROUND" }
    filter {}

    -- prebuild commands
    prebuildcommands {
        PY .. " ../../tools/building_script/py_prebuild.py pyllbc",
    }

    -- target name, target prefix, extension
    targetname "llbc"
    targetprefix ""
    filter { "system:windows" }
        targetextension ".pyd"
    filter {}

    -- links
    -- link llbc library
    libdirs { LLBC_OUTPUT_DIR }

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

    -- link python library
    if string.len(PYLIB_LIB_DIR) > 0 then
        libdirs { PYLIB_LIB_DIR }
    else
        filter { "system:windows", "architecture:x86" }
            if PYLIB_PY_VER == 2 then
                libdirs { PY_WRAP_PATH .. "/Python2.7.8/Libs/Win/32" }
            elseif PYLIB_PY_VER == 3 then
                libdirs { PY_WRAP_PATH .. "/Python3.8.5/Libs/Win/32" }
            end
        filter {}
        filter { "system:windows", "architecture:x64" }
            if PYLIB_PY_VER == 2 then
                libdirs { PY_WRAP_PATH .. "/Python2.7.8/Libs/Win/64" }
            elseif PYLIB_PY_VER == 3 then
                libdirs { PY_WRAP_PATH .. "/Python3.8.5/Libs/Win/64" }
            end
        filter {}
    end

    if string.len(PYLIB_LIB_NAME) > 0 then
        links { PYLIB_LIB_NAME }
    else
        -- in windows platform, python library use the python library file in the repo
        filter { "system:windows", "configurations:debug*" }
            if PYLIB_PY_VER == 2 then
                links { "python27_d" }
            elseif PYLIB_PY_VER == 3 then
                links { "python38_d" }
            end
        filter {}
        filter { "system:windows", "configurations:release*" }
            if PYLIB_PY_VER == 2 then
                links { "python27" }
            elseif PYLIB_PY_VER == 3 then
                links { "python38" }
            end
        filter {}

        -- in non-windows platform, python library default named: python2.7
        filter { "system:not windows" }
            if PYLIB_PY_VER == 2 then
                links { "python2.7" }
            elseif PYLIB_PY_VER == 3 then
                links { "python3.8" }
            end
        filter {}
    end

    -- Enable c++11 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++11",
	    "-Wno-deprecated-register",
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
-- csharp wrap library(csllbc) native library compile setting
project "csllbc_native"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- dependents
    dependson {
        "llbc",
    }

    -- library suffix
    targetprefix "lib"

    -- files
    files {
        CS_WRAP_PATH .. "/native/**.h",
        CS_WRAP_PATH .. "/native/**.cpp",
    }

    -- includedirs
    includedirs {
        CORELIB_PATH .. "/include",
        CS_WRAP_PATH .. "/native/include",
    }

    -- links
    libdirs {
        LLBC_OUTPUT_DIR,
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

    -- disable warnings
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
-- csharp wrap library(csllbc) compile setting
project "csllbc"
    -- language, kind
    kind "SharedLib"
    language "c#"

    -- files
    files {
        CS_WRAP_PATH .. "/csharp/**.cs",
    }

    -- dependents
    dependson {
        "llbc",
        "csllbc_native",
    }

    -- set unsafe flag
    clr "Unsafe"

    -- prebuild commands
    filter { "system:windows" }
        prebuildcommands {
            PY .. ' -c "import os;print(os.getcwd())"',
            PY .. " ../../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
            PY .. " ../../../wrap/csllbc/csharp/script_tools/gen_errno_code.py",
        }
    filter {}
    filter { "system:not windows" }
        prebuildcommands {
            PY .. ' -c "import os;print(os.getcwd())"',
            PY .. " ../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
            PY .. " ../../wrap/csllbc/csharp/script_tools/gen_errno_code.py",
        }
    filter {}

    -- postbuild commands
    filter { "system:not windows" }
        postbuildcommands {
            PY .. " ../../wrap/csllbc/csharp/script_tools/gen_dll_cfg.py ../../output/" .. _ACTION,
        }
    filter {}

    -- defines
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

    -- links
    filter {}
    links {
        "System",
        "System.Net",
        "System.Core",
    }

-- ****************************************************************************
-- csharp wrap library(csllbc) testsuite compile setting
project "csllbc_testsuite"
    -- language, kind
    kind "ConsoleApp"
    language "c#"

    -- dependents
    dependson {
        "llbc",
        "csllbc_native",
        "csllbc",
    }

    -- files
    files {
        CS_WRAP_PATH .. "/testsuite/**.cs",
    }

    -- links
    links {
        "System",
        "System.Net",
        "System.Core",
        "csllbc",
    }

group "wrap/lullbc"

-- ****************************************************************************
-- luasrc library(liblua) compile setting
local LUA_SRC_PATH = "../../wrap/lullbc/lua"
project "lullbc_lualib"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- files
    files {
        LUA_SRC_PATH .. "/*.h",
        LUA_SRC_PATH .. "/*.c",
    }
    removefiles {
        LUA_SRC_PATH .. "/lua.c",
        LUA_SRC_PATH .. "/luac.c",
        LUA_SRC_PATH .. "/onelua.c",
    }

    -- defines
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

    -- links
    filter { "system:not windows" }
        links { "dl" }
    filter {}

    -- target name, target prefix
    targetname "lua"
    targetprefix "lib"

-- lua executable compile setting
local LUA_SRC_PATH = LU_WRAP_PATH .. "/lua"
project "lullbc_luaexec"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

    -- files
    files {
        LUA_SRC_PATH .. "/*.h",
        LUA_SRC_PATH .. "/lua.c",
    }

    -- defines
    defines {
        "LUA_COMPAT_5_1",
        "LUA_COMPAT_5_2",
        "LUA_COMPAT_5_3",
    }

    -- dependents
    dependson {
        "lullbc_lualib"
    }

    -- links 
    libdirs { 
        LLBC_OUTPUT_DIR,
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
 
    -- target name, target prefix
    targetname "lua"

-- lua wrap library(lullbc) compile setting
-- import lualib_setting
package.path = package.path .. ";" .. "../../wrap/lullbc/?.lua"
local LUALIB_SETTING = require "lualib_setting"
local LUALIB_INCL_PATH = (LUALIB_SETTING.lua_path[1] ~= nil and LUALIB_SETTING.lua_path[1] ~= "") and LUALIB_SETTING.lua_path[1] or LUA_SRC_PATH
local LUALIB_LIB_DIR = (LUALIB_SETTING.lua_path[2] ~= nil and LUALIB_SETTING.lua_path[2] ~= "") and LUALIB_SETTING.lua_path[2] or LLBC_OUTPUT_DIR
project "lullbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- dependents
    dependson {
        "llbc",
        "lullbc_lualib",
        "lullbc_luaexec",
    }

    -- files
    files {
        LU_WRAP_PATH .. "/lua/*.h",

        LU_WRAP_PATH .. "/include/**.h",
        LU_WRAP_PATH .. "/src/**.h",
        LU_WRAP_PATH .. "/src/**.c",
        LU_WRAP_PATH .. "/src/**.cpp",

        LU_WRAP_PATH .. "/script/**.lua",

        LU_WRAP_PATH .. "/testsuite/**.lua",
        LU_WRAP_PATH .. "/testsuite/**.cfg",
    }

    -- define targetextension
    filter { "system:macosx" }
        targetextension ".so"
    filter {}

    -- includedirs
    includedirs {
        LUALIB_INCL_PATH,
        CORELIB_PATH .. "/include",
        LU_WRAP_PATH .. "/include",
        LU_WRAP_PATH,
    }

    -- defines
    filter { "system:windows", "action:vs2013 and vs2015 and vs2017" }
    defines {
        "HAVE_ROUND",
    }
    filter {}

    -- prebuild commands
    filter { "configurations:debug*" }
        prebuildcommands {
            PY .. " ../../tools/building_script/lu_prebuild.py lullbc debug",
        }
        postbuildcommands {
            PY .. string.format(' ../../tools/building_script/lu_postbuild.py %s %s "%s"', "lullbc", "debug", LLBC_OUTPUT_DIR),
        }
    filter {}

    filter { "configurations:release*" }
        prebuildcommands {
            PY .. " ../../tools/building_script/lu_prebuild.py lullbc release",
        }
        postbuildcommands {
            PY .. string.format(' ../../tools/building_script/lu_postbuild.py %s %s "%s"', "lullbc", "release", LLBC_OUTPUT_DIR),
        }
    filter {}

    -- target name, target prefix, extension
    targetname "_lullbc"
    targetprefix ""

    -- links 
    libdirs { 
        LLBC_OUTPUT_DIR,
        LUALIB_LIB_DIR,
    }
    filter { "configurations:debug*", "system:windows" }
        links {
            "libllbc_debug",
            (LUALIB_SETTING.lua_path[3] ~= nil and LUALIB_SETTING.lua_path[3] ~= "") and LUALIB_SETTING.lua_path[3] or "liblua_debug",
        }
    filter {}
    filter { "configurations:debug*", "system:not windows" }
        links {
            "llbc_debug",
            (LUALIB_SETTING.lua_path[3] ~= nil and LUALIB_SETTING.lua_path[3] ~= "") and LUALIB_SETTING.lua_path[3] or "lua_debug",
        }
    filter {}
    filter { "configurations:release*", "system:windows" }
        links {
            "libllbc",
            (LUALIB_SETTING.lua_path[3] ~= nil and LUALIB_SETTING.lua_path[3] ~= "") and LUALIB_SETTING.lua_path[3] or "liblua",
        }
    filter {}
    filter { "configurations:release*", "system:not windows" }
        links {
            "llbc",
            (LUALIB_SETTING.lua_path[3] ~= nil and LUALIB_SETTING.lua_path[3] ~= "") and LUALIB_SETTING.lua_path[3] or "lua",
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

    -- linkoptions
    filter { "system:macosx" }
    	linkoptions {
            "-undefined dynamic_lookup",
        }
    filter {}

group ""
