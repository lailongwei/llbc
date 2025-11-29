--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc library(included all wrap libraries) premake script define.

require "premake5_cfg"

-- Before gen Makefiles, dump system & architecture info.
print(string.format(
    '-------- system type: %s, arch type: %s --------', llbc_system_type, llbc_arch_type))

-- llbc project fast include function.
function include_llbc_core_lib()
    -- includedirs.
    includedirs {
        llbc_core_lib_path .. "/include",
    }

    -- links - llbc core lib dependency libs.
    libdirs { llbc_output_dir }
    filter { "system:linux" }
        links {
            "dl",
			"pthread",
        }
    filter { "system:windows" }
        links {
            "ws2_32",
        }
    filter {}

    -- links - llbc core lib.
    if llbc_system_type ~= llbc_system_types.windows then
        if llbc_enable_asan_support then
            filter { "configurations:debug*" }
                links { "llbc_asan_debug" }
            filter { "configurations:release*" }
                links { "llbc_asan" }
        else
            filter { "configurations:debug*" }
                links { "llbc_debug" }
            filter { "configurations:release*" }
                links { "llbc" }
        end
    else
        filter { "configurations:debug*" }
            links { "libllbc_debug" }
        filter { "configurations:release*" }
            links { "libllbc" }
    end
    filter {}
end

-- llbc workspace define.
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
        architecture(llbc_arch_type)
    filter { "configurations:*64" }
        architecture(llbc_arch_type .. llbc_arch_connect_char .. '64')
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

    -- set C++ standard as C++17
    cppdialect "C++17"

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
    if llbc_system_type ~= llbc_system_types.windows and llbc_enable_asan_support then
        filter { "configurations:debug*", "language:c++" }
            targetsuffix "_asan_debug"
        filter {}
        filter { "configurations:release*", "language:c++" }
            targetsuffix "_asan"
        filter {}

        filter { "configurations:debug*", "language:non c++" }
            targetsuffix "_debug"
        filter {}
    else
        filter { "configurations:debug*" }
            targetsuffix "_debug"
        filter {}
    end

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

    -- enable asan support
    if llbc_system_type ~= llbc_system_types.windows and llbc_enable_asan_support then
        filter { "system:not windows", "language:c++" }
            buildoptions { "-fsanitize=address", "-g" }
            linkoptions { "-fsanitize=address" }
        filter {}
    end

    -- for macosx, add Foundation framework.
    filter { "system:macosx", "language:c++" }
        linkoptions { "-framework Foundation" }
    filter {}
    -- for macosx, auto add rpath=@loader_path if is debug target.
    filter { "system:macosx", "language:c++", "configurations:debug*" }
        linkoptions { "-Wl,-rpath,@loader_path" }
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
        llbc_core_lib_path .. "/**.hpp",
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
            "unwind",
            "unwind-x86_64",
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

    -- Enable c++17 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++17",
        }
    filter {}

    -- Default hidden symbols.
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

    -- prebuild.
    local prebuild_script = '../../tools/building_script/llbc_prebuild.py'
    local prebuild_cmd = string.format('%s %s %%s %%s %s',
                                       llbc_py_exec_path, prebuild_script, _ACTION)
    filter { "configurations:debug32" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type, 'debug') }
    filter { "configurations:release32" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type, 'release') }
    filter { "configurations:debug64" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'debug') }
    filter { "configurations:release64" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'release') }
    filter {}

    -- Note: For now, disable auto update mechanism of LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD macro.
    -- if llbc_system_type ~= llbc_system_types.windows then
    --     prebuildcommands {
    --         string.format([=[grep -E 'LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD *%d' '%s' > /dev/null && sed -i 's|\(#define LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD\)\([[:space:]]*\)%d|\1\2%d|g' %s; echo 'do nothing' > /dev/null]=],
    --             llbc_enable_asan_support and 0 or 1,
    --             llbc_core_lib_path .. '/include/llbc/common/Config.h',
    --             llbc_enable_asan_support and 0 or 1,
    --             llbc_enable_asan_support and 1 or 0,          
    --             llbc_core_lib_path .. '/include/llbc/common/Config.h')
    --     }
    -- end

group "tests"
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

    -- include llbc core lib.
    include_llbc_core_lib()

    -- includedirs.
    includedirs {
        llbc_testsuite_path,
    }

    -- Enable c++17 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++17",
        }
    filter {}

    -- Specific debug directory.
    debugdir(llbc_output_dir)

    -- Copy all testcases config files to output directory.
    postbuildmessage("Copying all testcase config files to output directory...");
    local test_cfgs = {
        "core/config/test_ini.ini",
        "core/config/test_prop.properties",
        "core/log/LogTestCfg.cfg",
        "core/log/LogTestCfg.xml",
        "app/AppCfgTest.cfg",
        "app/AppCfgTest.properties",
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

-- ****************************************************************************
-- quick start project compile setting.
project "quick_start"
    -- laugnage, kind.
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
        llbc_quick_start_path .. "/**.h",
        llbc_quick_start_path .. "/**.cpp",
        llbc_quick_start_path .. "/**.xml",
        llbc_quick_start_path .. "/**.cfg",
        llbc_quick_start_path .. "/**.ini",
    }

    -- include llbc core lib.
    include_llbc_core_lib()

    -- includedirs.
    includedirs {
        llbc_quick_start_path,
    }

    -- Enable c++17 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++17",
        }
    filter {}

    -- Specific debug directory.
    debugdir(llbc_output_dir)

group "wrap"
-- ****************************************************************************
-- python wrap library(pyllbc) compile setting.
-- import pylib_setting.
package.path = package.path .. ";" .. "../../wrap/pyllbc/?.lua"
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
    local cpython_path = llbc_py_wrap_path .. "/cpython"
    files {
        llbc_py_wrap_path .. "/include/**.h",
        llbc_py_wrap_path .. "/src/**.h",
        llbc_py_wrap_path .. "/src/**.cpp",

        cpython_path .. "/Include/**.h",
        llbc_py_wrap_path .. "/script/**.py",
        llbc_py_wrap_path .. "/testsuite/**.py",
    }

    -- includedirs.
    includedirs {
        -- llbc core lib.
        llbc_core_lib_path .. "/include",

        -- pyllbc.
        llbc_py_wrap_path .. "/include",
        llbc_py_wrap_path,

        -- cpython.
        cpython_path .. "/Include",
    }
    -- cpython - pyconfig.h
    filter { "system:windows" }
        includedirs { cpython_path .. "/PC" }
    filter { "system:not windows" }
        includedirs { cpython_path }
    filter {}

    -- define HAVE_ROUND(only on vs2013, vs2015, vs2017 and later version visual studio IDEs).
    filter { "action:vs20*" }
        defines { "HAVE_ROUND" }
    filter {}

    -- prebuild & postbuild
    local prebuild_script = '../../tools/building_script/py_prebuild.py'
    local postbuild_script = '../../tools/building_script/py_postbuild.py'
    local prebuild_cmd = string.format('%s %s %%s %%s %s',
                                       llbc_py_exec_path, prebuild_script, _ACTION)
    local postbuild_cmd = string.format('%s %s %%s %%s %s',
                                        llbc_py_exec_path, postbuild_script, _ACTION)
    filter { "configurations:debug32" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type, 'debug') }
    postbuildcommands { string.format(postbuild_cmd, llbc_arch_type, 'debug') }
    filter { "configurations:release32" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type, 'release') }
    postbuildcommands { string.format(postbuild_cmd, llbc_arch_type, 'release') }
    filter { "configurations:debug64" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'debug') }
    postbuildcommands { string.format(postbuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'debug') }
    filter { "configurations:release64" }
    prebuildcommands { string.format(prebuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'release') }
    postbuildcommands { string.format(postbuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'release') }
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
    filter { "system:windows", "configurations:release*" }
        links { "libllbc" }
    filter {}

    filter { "system:not windows", "configurations:debug*" }
        links { "llbc_debug" }
    filter { "system:not windows", "configurations:release*" }
        links { "llbc" }
    filter {}

    -- set cpython libdir.
    filter { "system:windows", "architecture:x86" }
        libdirs { cpython_path .. "/PCbuild" }
    filter { "system:windows", "architecture:x64" }
        libdirs { cpython_path .. "/PCbuild/amd64" }
    filter { "system:not windows" }
        libdirs { cpython_path }
    filter {}

    -- link cpython lib.
    -- local py_ver_str = os_capture('python --version')
    -- local py_ver_match_str = '(%w+ )(%d+).(%d+).(%d+)'
    local py_ver_str = '2.7.18'
    local cpython_readme_file = io.open(llbc_py_wrap_path .. '/cpython/README', 'r')
    if not cpython_readme_file then
        io.stderr:write(string.format('Could not open cpython README file, set set python version to:%s\n', py_ver_str))
    else
        py_ver_str = cpython_readme_file:read()
        cpython_readme_file:close()
    end

    local py_ver_match_str = '(.*)(%d+).(%d+).(%d+).*'
    local py_major_ver = string.gsub(py_ver_str, py_ver_match_str, '%2')
    local py_minor_ver = string.gsub(py_ver_str, py_ver_match_str, '%3')

    filter { "system:windows", "configurations:debug*" }
        links { string.format("python%s%s_d", py_major_ver, py_minor_ver) }
    filter { "system:windows", "configurations:release*" }
        links { string.format("python%s%s", py_major_ver, py_minor_ver) }
    filter { "system:not windows" }
        links { string.format("python%s.%s", py_major_ver, py_minor_ver) }
    filter {}

    -- Enable c++17 support.
    filter { "system:not windows", "language:c++" }
        buildoptions {
            "-std=c++17",
            "-Wno-register"
        }
    filter {}

    -- Default hidden symbols.
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
            "-Wno-error=register",
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
    filter { "system:windows", "configurations:release*" }
        links {
            "libllbc",
        }
    filter { "system:not windows", "configurations:debug*" }
        links {
            "llbc_debug",
        }
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

    -- Enable c++17 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++17",
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
    filter { "configurations:debug*", "system:windows" }
        links { "liblua_debug" }
    filter { "configurations:release*", "system:windows" }
        links { "liblua" }
    filter { "configurations:debug*", "system:not windows" }
        links { "lua_debug" }
    filter { "configurations:release*", "system:not windows" }
        links { "lua" }
    filter {}
 
    -- target name, target prefix.
    targetname "lua"

    -- for macosx, fix premake5 not auto add @rpath bug(ignore debug* configuration).
    filter { "system:macosx", "configurations:release*" }
        linkoptions { "-Wl,-rpath,@loader_path" }
    filter {}

-- lua wrap library(lullbc) compile setting.
-- import lualib_setting.
package.path = package.path .. ";" .. "../../wrap/lullbc/?.lua"
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
        llbc_lua_src_path,
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

    -- prebuild & postbuild.
    local prebuild_script = llbc_building_script_dir .. '/lu_prebuild.py'
    local postbuild_script = llbc_building_script_dir .. '/lu_postbuild.py'
    local prebuild_cmd = string.format('%s %s %%s %%s %s', llbc_py_exec_path, prebuild_script, _ACTION)
    local postbuild_cmd = string.format('%s %s %%s %%s %s', llbc_py_exec_path, postbuild_script, _ACTION)
    filter { "configurations:debug32" }
        prebuildcommands { string.format(prebuild_cmd, llbc_arch_type, 'debug') }
        postbuildcommands { string.format(postbuild_cmd, llbc_arch_type, 'debug') }
    filter { "configurations:release32" }
        prebuildcommands { string.format(prebuild_cmd, llbc_arch_type, 'release') }
        postbuildcommands { string.format(postbuild_cmd, llbc_arch_type, 'release') }
    filter { "configurations:debug64" }
        prebuildcommands { string.format(prebuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'debug') }
        postbuildcommands { string.format(postbuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'debug') }
    filter { "configurations:release64" }
        prebuildcommands { string.format(prebuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'release') }
        postbuildcommands { string.format(postbuild_cmd, llbc_arch_type .. llbc_arch_connect_char .. '64', 'release') }
    filter {}

    -- target name, target prefix, extension.
    targetname "_lullbc"
    targetprefix ""

    -- links.
    libdirs { llbc_output_dir }

    filter { "configurations:debug*", "system:windows" }
        links {
            "libllbc_debug",
            "liblua_debug",
        }
    filter {}
    filter { "configurations:debug*", "system:linux" }
        links {
            "llbc_debug",
            "lua_debug",
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
            "liblua",
        }
    filter {}
    filter { "configurations:release*", "system:linux" }
        links {
            "llbc",
            "lua",
        }
    filter {}
    filter { "configurations:release*", "system:macosx" }
        links {
            "llbc",
        }
    filter {}

    -- Enable c++17 support.
    filter { "system:not windows" }
        buildoptions {
            "-std=c++17",
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
