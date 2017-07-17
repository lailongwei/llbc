# -*- coding: utf-8 -*-
"""
pyllbc专用native方法收集器, 完成c/c++native方法收集, 并整合到对应的脚本语言中
"""

import re
import os
from os import path as op

from cpputils import *

from c import Cfg
from native_method_collector.base_native_method_collector import BaseNativeMethodCollector


class PyNativeMethodCollector(BaseNativeMethodCollector):
    def __init__(self, search_path, classname_base=None, filename_base=None):
        super(PyNativeMethodCollector, self).__init__(search_path, classname_base, filename_base)
        self.kwargsmatch_re = re.compile(r"^ *LLBC_EXTERN_C +PyObject *\* *"
                                         r"_pyllbc_([a-zA-Z0-9_]+)\( *(PyObject *\*[^,]*, *){2}PyObject *\*")
        self.argsmatch_re = re.compile(r"^ *LLBC_EXTERN_C +PyObject *\* *_pyllbc_([a-zA-Z0-9_]+)\( *PyObject *\*")

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

        # 创建c++类
        cls_name = self._build_cpp_cls_name()
        cls = CppClass(cls_name)

        first_data = True
        ctor = CppFun(cls_name, rtn='', visit=CppVisit(CppVisit.PUBLIC))
        for meth_name, meth in methods.iteritems():
            ctor.addstmt('{0}.ml_name = {1};'.format(meth_name, meth['name']))
            ctor.addstmt('{0}.ml_meth = {1};'.format(meth_name, meth['meth']))
            ctor.addstmt('{0}.ml_flags = {1};'.format(meth_name, meth['flags']))
            ctor.addstmt('{0}.ml_doc = {1};'.format(meth_name, meth['doc']))

            visit = CppVisit(CppVisit.PUBLIC) if first_data else CppVisit(CppVisit.NONE)
            cls.adddata(CppData('::PyMethodDef', meth_name, visit=visit))

            first_data = False

        cls.addmethod(ctor)

        cpp_file.addcls(cls)
        cpp_file.adddef('pyllbc_s_{0}Methods LLBC_Singleton< ::pyllbc_{0}Methods, LLBC_DummyLock>::Instance()'
                        .format(self.search_base))
        cpp_file.build()

        return True

    def _parse_file(self, fpath):
        rtn = {}
        with open(fpath) as f:
            for line in f.readlines():
                func_flags = 'METH_VARARGS'
                match = self.kwargsmatch_re.match(line)
                if not match:
                    match = self.argsmatch_re.match(line)
                    if not match:
                        continue
                else:
                    func_flags += ' | METH_KEYWORDS'

                meth = match.group(1)
                md = {'name': '"' + meth + '"',
                      'meth': '(PyCFunction)_pyllbc_{0}'.format(meth),
                      'flags': func_flags,
                      'doc': '"pyllbc library method/function"'}
                rtn[meth] = md

        return rtn
