--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script define.

-- #########################################################################
-- Global compile settings

-- All libraries output directory
local LLBC_OUTPUT_DIR = "../../../output/" .. _ACTION

-- Some third party libraries paths define
-- Format [2]: include path, [2]: lib path [3]: lib name
local PYTHON_LIB = { "/usr/local/include/python2.7", "/usr/local/lib", "python2.7" }
-- #########################################################################

workspace "llbc"
    configurations { "release", "debug" }
    location ("../../../build/" .. _ACTION)

    -- target directory define
    targetdir (LLBC_OUTPUT_DIR)

    -- remove independent platforms
    filter "system:linux"
        platforms { "linux" }
    filter "system:win*"
        removeplatforms { "win32", "win64" }

    -- defines
    filter { "configurations:debug" }
        defines {
            "DEBUG"
        }
    filter { "system:linux", "language:c++" }
        defines {
            "LINUX"
        }

    -- optimize
    filter { "configurations:debug" }
        optimize "Debug"
    filter { "configurations:release" }
        optimize "On"

    -- architecture
    filter "platforms:win32"
        architecture "x32"
    filter "platforms:win64 or linux"
        architecture "x64"


-- ****************************************************************************
-- llbc core library compile setting
project "llbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- files
    files {
        "../../../llbc/**.h",
        "../../../llbc/**.c",
        "../../../llbc/**.cpp",
    }

    -- links
    filter "system:linux"
        links {
            "rt",
            "uuid",
        }

    -- debug target suffix define
    filter "configurations:debug"
        targetsuffix "_debug"


-- ****************************************************************************
-- core library testsuite compile setting
project "testsuite"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

    -- files
    files {
        "../../../testsuite/**.h",
        "../../../testsuite/**.cpp",
    }

    -- includedirs
    includedirs {
        "../../../llbc/include",
        "../../../testsuite",
    }

    -- links
    libdirs { LLBC_OUTPUT_DIR }
    filter "configurations:debug"
        links {
            "dl",
            "llbc_debug",
        }
    filter "configurations:release"
        links {
            "dl",
            "llbc",
        }

    -- debug target suffix define
    filter "configurations:debug"
        targetsuffix "_debug"

-- ****************************************************************************
-- python wrap library(pyllbc) compile setting
project "pyllbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- files
    files {
        "../../../wrap/pyllbc/**.h",
        "../../../wrap/pyllbc/**.cpp",
    }

    -- includedirs
    includedirs {
        PYTHON_LIB[1],

        "../../../llbc/include",
        "../../../wrap/pyllbc/include",
        "../../../wrap/pyllbc",
    }

    -- prebuild commands
    prebuildcommands {
        "python ../../wrap/pyllbc/script_tools/prebuild.py",
    }

    -- target name, target prefix
    targetname "llbc"
    targetprefix ""

    -- links 
    libdirs { 
        LLBC_OUTPUT_DIR,
        PYTHON_LIB[2],
    }
    filter "configurations:debug"
        links {
            PYTHON_LIB[3],
            "llbc_debug",
        }
    filter "configurations:release"
        links {
            PYTHON_LIB[3],
            "llbc",
        }

    -- debug target suffix define
    filter "configurations:debug"
        targetsuffix "_debug"

-- ****************************************************************************
-- csharp wrap library(csllbc) native library compile setting
project "csllbc_native"
    -- language, kind
    kind "SharedLib"
    language "c++"

    -- files
    files {
        "../../../wrap/csllbc/native/**.h",
        "../../../wrap/csllbc/native/**.cpp",
    }

    -- includedirs
    includedirs {
        "../../../llbc/include",
        "../../../wrap/csllbc/native/include",
    }

    -- links
    libdirs {
        LLBC_OUTPUT_DIR,
    }
    filter "configurations:debug"
        links {
            "llbc_debug",
        }
    filter "configurations:release"
        links {
            "llbc",
        }

    -- debug target suffix define
    filter "configurations:debug"
        targetsuffix "_debug"

    -- disable warnings
    filter "system:not windows"
        disablewarnings {
            "attributes"
        }

-- ****************************************************************************
-- csharp wrap library(csllbc) compile setting
project "csllbc"
    -- language, kind
    kind "SharedLib"
    language "c#"

    -- files
    files {
        "../../../wrap/csllbc/csharp/**.cs",
    }


    -- set unsafe flag
    clr "Unsafe"

    -- prebuild commands
    prebuildcommands {
        "python ../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
    }

    -- defines
    filter "system:linux"
        defines {
            "CSLLBC_TARGET_PLATFORM_LINUX",
        }
    filter "system:windows"
        defines {
            "CSLLBC_TARGET_PLATFORM_WIN32",
        }


    -- links
    filter "configurations:debug"
        links {
            "System",
            "System.Core",
            "System.Net",
        }
    filter "configurations:release"
        links {
            "System",
            "System.Core",
            "System.Net",
        }

    -- debug target suffix define
    filter "configurations:debug"
        targetsuffix "_debug"

-- ****************************************************************************
-- csharp wrap library(csllbc) testsuite compile setting
project "csllbc_testsuite"
    -- language, kind
    kind "ConsoleApp"
    language "c#"

    -- files
    files {
        "../../../wrap/csllbc/testsuite/**.cs",
    }

    -- links
    filter "configurations:release"
        links {
            "csllbc",
    
            "System",
            "System.Core",
            "System.Net",
        }
    
    -- debug target suffix define
    filter "configurations:debug"
        targetsuffix "_debug"

