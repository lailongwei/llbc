# -*- coding: utf-8 -*-
"""
lullbc专用native方法收集器, 完成c/c++native方法收集, 并整合到对应的脚本语言中
"""
import os
from os import path as op

import re

from c import Cfg
from cpputils import *
from native_method_collector.base_native_method_collector import BaseNativeMethodCollector


class LuNativeMethodCollector(BaseNativeMethodCollector):
    def __init__(self, search_path, classname_base=None, filename_base=None):
        super(LuNativeMethodCollector, self).__init__(search_path, classname_base, filename_base)
        self._meth_re = re.compile(
            r'\s*LULLBC_LUA_METH\s+int\s+(_lullbc_([a-zA-Z0-9_]+))\s*\(\s*lua_State\s*\*\s*[a-zA-Z0-9_]*\s*\)\s*')
        self._anno_re = re.compile(r'\s*//.*')
        self._doc_re = re.compile(r'\s*//\s*[Aa][Pp][Ii]\s*:\s*([a-zA-Z0-9_]+)')

    def build(self):
        """构建方法文件"""
        # 确认是否可构建
        if not self._buildable():
            return False

        # 创建cpp文件对象, 用于存放自动生成代码
        proj_name = self.proj_name
        cpp_file = self._build_cpp_file()

        # 取得所有方法
        methods = {}
        code_path = Cfg.getcodepath()
        r = self._build_filematch_re()
        for root, dirs, files in os.walk(self.search_path):
            for f in files:
                fpath = op.join(root, f)
                if '.svn' in fpath or '.git' in fpath:
                    continue
                elif fpath == cpp_file.fpath:
                    continue

                if not r.match(f):
                    continue

                file_methods = self._parse_file(fpath)
                if not file_methods:
                    continue

                incl = fpath[len(op.dirname(code_path)) + 1:]
                cpp_file.addincl(incl)

                methods.update(file_methods)

        # 构造luaL_Reg[] 定义
        def _build_luareg(meth):
            if not meth:
                return '\n    {NULL, NULL}'
            else:
                return '\n    {{"{0}", {1}}},'.format(meth['lua_name'], meth['name'])

        luareg = 'static luaL_Reg lullbc_NativeMethods[] = {'
        for meth in methods.itervalues():
            luareg += _build_luareg(meth)
        luareg += _build_luareg(None)
        luareg += '\n}'

        cpp_file.addcodeline(luareg)
        cpp_file.build()

        return True

    def _parse_file(self, fpath):
        meths = {}
        with open(fpath, 'r') as f:
            lines = f.readlines()
            for idx in xrange(len(lines)):
                line = lines[idx]
                m = self._meth_re.match(line)
                if not m:
                    continue

                meth = m.group(1)
                lua_meth = m.group(2)
                anno_idx = idx - 1
                while anno_idx >= 0:
                    if not self._anno_re.match(lines[anno_idx]):
                        anno_idx -= 1
                        continue

                    m2 = self._doc_re.match(lines[idx - 1])
                    if m2 is not None:
                        lua_meth = m2.group(1)
                    anno_idx -= 1

                meths[meth] = {'name': meth, 'lua_name': lua_meth}
        return meths
