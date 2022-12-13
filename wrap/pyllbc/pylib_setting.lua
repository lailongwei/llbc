local pylib_setting = {
    -- py_ver = 2, -- 2: python2x, 3: python3x, default is 2, Note: this setting option always used no matter 'use_setting' option.
    -- py_path = {"", "", ""}, -- python path setting, 1: include path, 2: library path, 3: library name
    py_ver = 2, -- 2: python2x, 3: python3x, default is 2, Note: this setting option always used no matter 'use_setting' option.
    py_path = {"/data/home/user00/workspace/git_projs/Python-2.7.18/Include",
               "/data/home/user00/workspace/git_projs/Python-2.7.18",
               "python2.7"}, -- python path setting, 1: include path, 2: library path, 3: library name
}

return pylib_setting
