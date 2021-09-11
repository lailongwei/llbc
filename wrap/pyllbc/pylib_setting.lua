local pylib_setting = {
    py_ver = 2, -- 2: python2x, 3: python3x, default is 2, Note: this setting option always used no matter 'use_setting' option.
    use_setting = false, -- If use_setting is False, compile system will auto detect python
    py_path = {"", "", ""}, -- python path setting, 1: include path, 2: library path, 3: library name
}

return pylib_setting
