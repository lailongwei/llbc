# -*- coding: utf-8 -*-
"""
脚本代码整合生成器
"""


import os
import re
from os import path as op


from cfg import Cfg
from cpputils import *


class IntegratorBuilder(object):
    @staticmethod
    def build():
        """生成"""
        # Generate file head.
        cpp_file_path = op.join(Cfg.getcodepath(), 'pyllbc_scripts.h')
        cpp_file = CppFile(cpp_file_path, Cfg.getauthor(), Cfg.getver())
        cpp_file.addincl('pyllbc/common/LibHeader.h')
        cpp_file.addincl('pyllbc/common/ScriptIntegrator.h')

        # Generate constructor/destructor.
        class_name = 'pyllbc_Scripts'
        ctor = CppFun(class_name, rtn='', visit=CppVisit(CppVisit.PUBLIC))
        for root, dirs, files in os.walk(Cfg.getscriptpath()):
            for f in files:
                fpath = op.join(root, f)
                if '.svn' in fpath:
                    continue
                elif op.splitext(fpath)[1].lower() != '.py':
                    continue

                script_cnt = IntegratorBuilder.__transfer_to_cpp_str(fpath)
                ctor.addstmt('_scripts.insert(::std::make_pair("{}", new ::pyllbc_ScriptIntegrator({})));'
                             .format(op.basename(fpath), script_cnt))

        dtor = CppFun('~{}'.format(class_name), rtn='', visit=CppVisit(CppVisit.PUBLIC))
        dtor.addstmt('LLBC_STLHelper::DeleteContainer(_scripts);')

        clazz = CppClass(class_name)
        clazz.addmethod(ctor)
        clazz.addmethod(dtor)

        # Generate integrate method.
        integrate_meth = CppFun('Integrate', rtn='int', visit=CppVisit(CppVisit.PUBLIC))
        integrate_meth.addstmt("for (::std::map<LLBC_String, "
                               "::pyllbc_ScriptIntegrator *>::iterator it = _scripts.begin();")
        integrate_meth.addstmt('     it != _scripts.end();')
        integrate_meth.addstmt('     it++)')
        integrate_meth.addstmt('    if (it->second->Integrate() != LLBC_RTN_OK)')
        integrate_meth.addstmt('        return LLBC_RTN_FAILED;')
        integrate_meth.addstmt('')
        integrate_meth.addstmt('return LLBC_RTN_OK;')
        clazz.addmethod(integrate_meth)

        # Generate data member.
        data_mem = CppData('::std::map<LLBC_String, ::pyllbc_ScriptIntegrator *>',
                           '_scripts',
                           False,
                           CppVisit(CppVisit.PRIVATE))
        clazz.adddata(data_mem)

        cpp_file.addcls(clazz)
        cpp_file.adddef('pyllbc_s_Scripts LLBC_Singleton<{}, LLBC_DummyLock>::Instance()'.format(class_name))

        cpp_file.build()

    @staticmethod
    def __transfer_to_cpp_str(script_file):
        """将脚本文件内容转换成合法的C++字符串表示"""
        t = ''
        with open(script_file) as f:
            for line in f.readlines():
                line = re.sub(r'\\', r'\\\\', line)
                line = re.sub(r'"', r'\\"', line)
                line = re.sub(r'\r?\n?', '', line)
                line += r'\n'

                # unified to use '\n' as new line character.
                t += '"' + line + '"\n'

        return t
