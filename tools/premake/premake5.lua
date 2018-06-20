--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script define.

-- #########################################################################
-- Global compile settings

-- python tool define
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil
local PY = IS_WINDOWS and "$(ProjectDir)../../tools/py.exe" or "python"

-- All libraries output directory
LLBC_OUTPUT_BASE_DIR = "../../output/" .. _ACTION
if IS_WINDOWS then
    LLBC_OUTPUT_DIR = LLBC_OUTPUT_BASE_DIR .. "/$(Configuration)"
else
    LLBC_OUTPUT_DIR = LLBC_OUTPUT_BASE_DIR .. "/$(config)"
end

-- Common functional functions define
-- Enable multithread compile
function enable_multithread_comp()
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild" }
    filter {}
end

-- Set optimize options.
function set_optimize_opts()
    filter { "configurations:debug*", "language:c++", "system:not windows" }
        buildoptions {
            "-ggdb -g",
        }
    filter {}

    filter { "configurations:debug*", "language:c++", "system:windows" }
        runtime "Debug"
        optimize "Debug"
    filter {}

    filter { "configurations:debug*", "language:not c++" }
        optimize "Debug"
    filter {}

    filter { "configurations:release*" }
        optimize "Speed"
    filter {}
end

-- zlib library:
local ZLIB_LIB = "../../llbc/3rd_party/zlib"
-- #########################################################################

workspace ("llbc_" .. _ACTION)
    -- location define
    location ("../../build/" .. _ACTION)
    -- target directory define
    targetdir (LLBC_OUTPUT_DIR)

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

    -- control symbols
    filter { "system:macosx", "language:c++" }
        symbols("On")
    filter {}

    -- characterset architecture
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

-- ****************************************************************************
-- llbc core library compile setting
project "llbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- symbols
    symbols "On"

    -- files
    files {
        "../../llbc/**.h",
        "../../llbc/**.c",
        "../../llbc/**.cpp",
    }
    filter { "system:macosx" }
    files {
        "../../llbc/**.mm",
    }

    -- includedirs
    filter {}
    includedirs {
        "../../llbc/include",
    }

    -- target prefix
    targetprefix "lib"

    -- links
    filter { "system:linux" }
        links {
            "rt",
            "uuid",
        }

    filter { "system:windows" }
        links {
            "ws2_32",
            "Mswsock",
        }

    filter { "system:macosx" }
        links {
            "iconv",
        }
    filter {}

    -- flags
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

-- ****************************************************************************
-- core library testsuite compile setting
project "testsuite"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

    -- symbols
    symbols "On"

    -- dependents
    dependson {
        "llbc",
    }

    -- files
    files {
        "../../testsuite/**.h",
        "../../testsuite/**.cpp",
    }

    -- includedirswrap\csllbc\csharp\script_tools
    includedirs {
        "../../llbc/include",
        "../../testsuite",
    }

    -- links
    libdirs { LLBC_OUTPUT_DIR }
    filter { "system:linux" }
        links {
            "dl",
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

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

    -- warnings
    filter { "system:not windows" }
        disablewarnings {
            "invalid-source-encoding",
        }
    filter {}

    -- optimize
    set_optimize_opts()

-- ****************************************************************************
-- python wrap library(pyllbc) compile setting
-- import pylib_setting
package.path = package.path .. ";" .. "../../wrap/pyllbc/?.lua"
local PYLIB_SETTING = require "pylib_setting"
local PYLIB_INCL_PATH = PYLIB_SETTING.use_setting and PYLIB_SETTING.py_path[1] or ''
local PYLIB_LIB_DIR = PYLIB_SETTING.use_setting and PYLIB_SETTING.py_path[2] or ''
local PYLIB_LIB_NAME = PYLIB_SETTING.use_setting and PYLIB_SETTING.py_path[3] or 'python27'
project "pyllbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- symbols
    symbols "On"

    -- dependents
    dependson {
        "llbc",
    }

    -- files
    files {
        "../../wrap/pyllbc/**.h",
        "../../wrap/pyllbc/**.cpp",

        "../../wrap/pyllbc/script/**.py",

        "../../wrap/pyllbc/testsuite/**.py",
    }

    -- includedirs
    includedirs {
        "../../llbc/include",

        "../../wrap/pyllbc/include",
        "../../wrap/pyllbc",
    }

    if string.len(PYLIB_INCL_PATH) > 0 then
        includedirs { PYLIB_INCL_PATH }
    else -- if not specific python include path, windows platform will use specific version python, other platforms will auto detect.
        filter { "system:windows" }
            includedirs { "../../wrap/pyllbc/Python2.7.8/Include" }
        filter {}

        filter { "system:not windows" }
            includedirs { "/usr/include/python2.7" }
        filter {}
    end

    -- define HAVE_ROUND(only on vs2013, vs2015, vs2017 and later version visual studio IDEs).
    filter { "action:vs2013 or vs2015 or vs2017" }
        defines { "HAVE_ROUND" }

    -- prebuild commands
    filter {}
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
            libdirs { "../../wrap/pyllbc/Python2.7.8/Libs/Win/32" }
        filter {}
        filter { "system:windows", "architecture:x64" }
            libdirs { "../../wrap/pyllbc/Python2.7.8/Libs/Win/64" }
        filter {}
    end

    if string.len(PYLIB_LIB_NAME) then
        links { PYLIB_LIB_NAME }
    else
        -- in windows platform, python library use the python library file in the repo
        filter { "system:windows", "configurations:debug*" }
            links { "python27_d" }
        filter {}
        filter { "system:windows", "configurations:release*" }
            links { "python27" }
        filter {}

        -- in non-windows platform, python library default named: python2.7
        filter { "system:not windows" }
            links { "python2.7" }
        filter {}
    end

    -- flags
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

-- ****************************************************************************
-- csharp wrap library(csllbc) native library compile setting
project "csllbc_native"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- symbols
    symbols "On"

    -- dependents
    dependson {
        "llbc",
    }

    -- library suffix
    targetprefix "lib"

    -- files
    files {
        "../../wrap/csllbc/native/**.h",
        "../../wrap/csllbc/native/**.cpp",
    }

    -- includedirs
    includedirs {
        "../../llbc/include",
        "../../wrap/csllbc/native/include",
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

    -- flags
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

    -- disable warnings
    filter { "system:not windows" }
        disablewarnings {
            "attributes"
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
        "../../wrap/csllbc/csharp/**.cs",
    }

    -- dependents
    dependson {
        "llbc",
        "csllbc_native",
    }

    -- set unsafe flag
    clr "Unsafe"

    -- prebuild commands
    prebuildcommands {
        PY .. ' -c "import os;print(os.getcwd())"',
        PY .. " ../../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
        PY .. " ../../../wrap/csllbc/csharp/script_tools/gen_errno_code.py",
    }

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

    -- optimize
    set_optimize_opts()

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
        "../../wrap/csllbc/testsuite/**.cs",
    }

    -- optimize
    set_optimize_opts()

    -- links
    links {
        "System",
        "System.Net",
        "System.Core",
        "csllbc",
    }

-- ****************************************************************************
-- luasrc library(liblua) compile setting
local LUA_SRC_PATH = "../../wrap/lullbc/lua"
project "lullbc_lualib"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- symbols
    symbols "On"

    -- files
    files {
        LUA_SRC_PATH .. "/*.h",
        LUA_SRC_PATH .. "/*.c",
    }
    removefiles {
        LUA_SRC_PATH .. "/lua.c",
        LUA_SRC_PATH .. "/luac.c",
    }

    -- defines
    defines {
        "LUA_COMPAT_5_1",
        "LUA_COMPAT_5_2",
    }
    filter { "system:windows" }
        defines { "LUA_BUILD_AS_DLL" }
    filter {}
    filter { "system:not windows" }
        defines { "LUA_USE_DLOPEN" }
    filter {}

    -- optimize
    set_optimize_opts()

    -- links
    filter { "system:not windows" }
        links { "dl" }
    filter {}

    -- target name, target prefix
    targetname "lua"
    targetprefix "lib"

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

-- lua executable compile setting
local LUA_SRC_PATH = "../../wrap/lullbc/lua"
project "lullbc_luaexec"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

    -- symbols
    symbols "On"

    -- files
    files {
        LUA_SRC_PATH .. "/*.h",
        LUA_SRC_PATH .. "/lua.c",
    }

    -- defines
    defines {
        "LUA_COMPAT_5_1",
        "LUA_COMPAT_5_2",
    }

    -- dependents
    dependson {
        "lullbc_lualib"
    }

    -- optimize
    set_optimize_opts()

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

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

-- lua wrap library(lullbc) compile setting
-- import lualib_setting
package.path = package.path .. ";" .. "../../wrap/lullbc/?.lua"
local LUALIB_SETTING = require "lualib_setting"
local LUALIB_INCL_PATH = LUALIB_SETTING.use_setting and LUALIB_SETTING.lua_path[1] or LUA_SRC_PATH
local LUALIB_LIB_DIR = LUALIB_SETTING.use_setting and LUALIB_SETTING.lua_path[2] or LLBC_OUTPUT_DIR
project "lullbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- symbols
    symbols "On"

    -- dependents
    dependson {
        "llbc",
        "lullbc_lualib",
        "lullbc_luaexec",
    }

    -- files
    files {
        "../../wrap/lullbc/lua/*.h",

        "../../wrap/lullbc/include/**.h",
        "../../wrap/lullbc/src/**.h",
        "../../wrap/lullbc/src/**.c",
        "../../wrap/lullbc/src/**.cpp",

        "../../wrap/lullbc/script/**.lua",

        "../../wrap/lullbc/testsuite/**.lua",
        "../../wrap/lullbc/testsuite/**.cfg",
    }

    -- define targetextension
    filter { "system:macosx" }
        targetextension ".so"
    filter {}

    -- includedirs
    includedirs {
        LUALIB_INCL_PATH,
        "../../llbc/include",
        "../../wrap/lullbc/include",
        "../../wrap/lullbc",
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

    -- optimize
    set_optimize_opts()

    -- links 
    libdirs { 
        LLBC_OUTPUT_DIR,
        LUALIB_LIB_DIR,
    }
    filter { "configurations:debug*", "system:windows" }
        links {
            "libllbc_debug",
            LUALIB_SETTING.use_setting and LUALIB_SETTING.lua_path[3] or "liblua_debug",
        }
    filter {}
    filter { "configurations:debug*", "system:not windows" }
        links {
            "llbc_debug",
            -- LUALIB_SETTING.use_setting and LUALIB_SETTING.lua_path[3] or "lua_debug",
        }
    filter {}
    filter { "configurations:release*", "system:windows" }
        links {
            "libllbc",
            LUALIB_SETTING.use_setting and LUALIB_SETTING.lua_path[3] or "liblua",
        }
    filter {}
    filter { "configurations:release*", "system:not windows" }
        links {
            "llbc",
            -- LUALIB_SETTING.use_setting and LUALIB_SETTING.lua_path[3] or "lua",
        }
    filter {}

    -- flags
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

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

