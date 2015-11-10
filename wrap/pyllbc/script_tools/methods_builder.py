# -*- coding: utf-8 -*-
"""
方法列表生成器
"""


import re
import os
from os import path as op

from cfg import Cfg
from cpputils import *


class MethodsBuilder(object):
    @staticmethod
    def build(search_path):
        """构建方法文件"""
        # Create cpp file.
        search_base = op.basename(search_path).capitalize()
        cpp_file_path = op.join(search_path, '_{}Methods.h'.format(search_base))
        cpp_file = CppFile(cpp_file_path, Cfg.getauthor(), Cfg.getver())
        cpp_file.addincl('pyllbc/common/LibHeader.h')

        # Get all methods
        methods = {}
        code_path = Cfg.getcodepath()
        c = re.compile(r'(_[a-zA-Z]+)+\.h')
        for root, dirs, files in os.walk(search_path):
            for f in files:
                fpath = op.join(root, f)
                if '.svn' in fpath:
                    continue
                elif fpath == cpp_file_path:
                    continue

                if c.match(f):
                    file_methods = MethodsBuilder.__parse_file(fpath)
                    if file_methods:
                        incl = fpath[len(op.dirname(code_path)) + 1:]
                        cpp_file.addincl(incl)

                        methods.update(file_methods)

        # Create cpp class.
        cls_name = 'pyllbc_{}Methods'.format(search_base)
        cls = CppClass(cls_name)

        first_data = True
        ctor = CppFun(cls_name, rtn='', visit=CppVisit(CppVisit.PUBLIC))
        for meth_name, meth in methods.iteritems():
            ctor.addstmt('{}.ml_name = {};'.format(meth_name, meth['name']))
            ctor.addstmt('{}.ml_meth = {};'.format(meth_name, meth['meth']))
            ctor.addstmt('{}.ml_flags = {};'.format(meth_name, meth['flags']))
            ctor.addstmt('{}.ml_doc = {};'.format(meth_name, meth['doc']))

            visit = CppVisit(CppVisit.PUBLIC) if first_data else CppVisit(CppVisit.NONE)
            cls.adddata(CppData('::PyMethodDef', meth_name, visit=visit))

            first_data = False

        cls.addmethod(ctor)

        cpp_file.addcls(cls)
        cpp_file.adddef('pyllbc_s_{0}Methods LLBC_Singleton< ::pyllbc_{0}Methods, LLBC_DummyLock>::Instance()'
                        .format(search_base))
        cpp_file.build()

    @staticmethod
    def __parse_file(fpath):
        rtn = {}
        with open(fpath) as f:
            c2 = re.compile(r"^ *LLBC_EXTERN_C "
                            r"+PyObject *\* *_pyllbc_([a-zA-Z0-9_]+)\( *(PyObject *\*[^,]*, *){2}PyObject *\*")
            c1 = re.compile(r"^ *LLBC_EXTERN_C +PyObject *\* *_pyllbc_([a-zA-Z0-9_]+)\( *PyObject *\*")
            for line in f.readlines():
                func_flags = 'METH_VARARGS'
                match = c2.match(line)
                if not match:
                    match = c1.match(line)
                    if not match:
                        continue
                else:
                    func_flags += ' | METH_KEYWORDS'

                meth = match.group(1)
                md = {'name': '"'+meth+'"', 'meth': '(PyCFunction)_pyllbc_{}'.format(meth), 'flags': func_flags,
                      'doc': '"pyllbc library method/function"'}
                rtn[meth] = md

        return rtn
