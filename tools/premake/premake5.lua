--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script define.

-- #########################################################################
-- Global compile settings

-- All libraries output directory
local LLBC_OUTPUT_DIR = "../../output/" .. _ACTION

-- Some third party libraries paths define
-- Python library: format [2]: include path, [2]: lib path [3]: lib name
local PYTHON_LIB = { "/usr/local/include/python2.7", "/usr/local/lib", "python2.7" }

-- zlib library:
local ZLIB_LIB = "../../llbc/3rd_party/zlib"
-- #########################################################################

workspace ("llbc_" .. _ACTION)
    -- location define
    location ("../../build/" .. _ACTION)
    -- target directory define
    targetdir (LLBC_OUTPUT_DIR)

    -- configurations
    configurations { "release", "debug" }

    -- defines
    filter { "configurations:debug*" }
        defines {
            "DEBUG"
        }
    filter { "system:windows", "language:c++" }
        defines {
            "WIN32",
        }
    filter { "system:linux", "language:c++" }
        defines {
            "LINUX"
        }
    filter { "system:macosx", "language:c++" }
        defines {
            "TARGET_OS_MAC",
        }

    -- optimize
    filter { "configurations:debug*" }
        optimize "Debug"
    filter { "configurations:release*" }
        optimize "On"

    -- architecture
    filter { "system:not windows" }
        architecture "x64"
    filter { "system:windows" }
        architecture "x86"

    -- characterset
    filter { "language:c++" }
        characterset "MBCS"

-- ****************************************************************************
-- llbc core library compile setting
project "llbc"
    -- language, kind
    language "c++"
    kind "SharedLib"

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
        ZLIB_LIB .. "/include",
        "../../llbc/include",
    }

    -- target prefix
    targetprefix "lib"

    -- links
    filter { "system:linux" }
        libdirs {
            ZLIB_LIB .. "/lib/linux"
        }
        links {
            "rt",
            "uuid",
        }

    filter { "system:windows" }
        libdirs {
            ZLIB_LIB .. "/lib/win"
        }
        links {
            "ws2_32",
            "Mswsock",
        }
    filter { "system:windows", "configurations:debug*", "architecture:x86" }
        links {
            "zlibwapi_debug",
        }
    filter { "system:windows", "configurations:release*", "architecture:x86" }
        links {
            "zlibwapi",
        }
    filter { "system:windows", "configurations:debug*", "architecture:x64" }
        links {
            "zlibwapi_debug_64",
        }
    filter { "system:windows", "configurations:release*", "architecture:x64" }
        links {
            "zlibwapi_64",
        }

    filter { "system:macosx" }
        links {
            "iconv",
        }

    -- flags
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"


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
        "../../testsuite/**.h",
        "../../testsuite/**.cpp",
    }

    -- includedirs
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

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"

-- ****************************************************************************
-- python wrap library(pyllbc) compile setting
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
        "../../wrap/pyllbc/**.h",
        "../../wrap/pyllbc/**.cpp",
    }

    -- includedirs
    includedirs {
        PYTHON_LIB[1],

        "../../wrap/pyllbc/Python2.7.8/Include",

        "../../llbc/include",

        "../../wrap/pyllbc/include",
        "../../wrap/pyllbc",
    }

    -- prebuild commands
    filter {}
    prebuildcommands {
        "python ../../wrap/pyllbc/script_tools/prebuild.py",
    }

    -- target name, target prefix
    targetname "llbc"
    targetprefix ""

    -- links 
    libdirs { 
        LLBC_OUTPUT_DIR,
    }
    filter { "system:linux" }
        libdirs {
            PYTHON_LIB[2],
        }
        links {
            PYTHON_LIB[3],
        }
    filter { "system:linux", "configurations:debug*" }
        links {
            "llbc_debug",
        }
    filter { "system:linux", "configurations:release*" }
        links {
            "llbc",
        }

    filter { "system:windows", "architecture:x86" }
        libdirs {
            "../../wrap/pyllbc/Python2.7.8/Libs/Win/32",
        }
    filter { "system:windows", "architecture:x64" }
        libdirs {
            "../../wrap/pyllbc/Python2.7.8/Libs/Win/64",
        }

    filter { "system:windows", "configurations:debug*" }
        links {
            "python27_d",
            "libllbc_debug",
        }
    filter { "system:windows", "configurations:release*" }
        links {
            "python27",
            "libllbc",
        }

    -- debug target suffix define
    filter { "configurations:debug" }
        targetsuffix "_debug"

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
    filter { "system:linux", "configurations:debug*" }
        links {
            "llbc_debug",
        }
    filter { "system:linux", "configurations:release*" }
        links {
            "llbc",
        }
    filter { "system:windows", "configurations:debug*" }
        links {
            "libllbc_debug",
        }
    filter { "system:windows", "configurations:release*" }
        links {
            "libllbc",
        }

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"

    -- disable warnings
    filter { "system:not windows" }
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
        "python ../../wrap/csllbc/csharp/script_tools/gen_native_code.py",
    }

    -- defines
    filter { "system:linux" }
        defines {
            "CSLLBC_TARGET_PLATFORM_LINUX",
        }
    filter { "system:windows" }
        defines {
            "CSLLBC_TARGET_PLATFORM_WIN32",
        }

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

    -- links
    links {
        "System",
        "System.Net",
        "System.Core",
        "csllbc",
    }

