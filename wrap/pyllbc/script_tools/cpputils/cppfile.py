# -*- coding: utf-8 -*-
"""
CPP文件简单封装,用于生成cpp文件.
"""


from os import path as op
from time import time, localtime, strftime


class CppFile(object):
    def __init__(self, fpath, author, ver):
        self.__fpath = fpath
        self.__author = author
        self.__ver = ver

        self.__incls = []
        self.__clses = []
        self.__funcs = []
        self.__datas = []
        self.__defs = []

    def addincl(self, incl):
        """增加新头文件包含"""
        self.__incls.append(incl.replace('\\', '/'))

    def addcls(self, cls):
        """增加新类定义"""
        self.__clses.append(cls)

    def addfunction(self, func):
        """增加新函数定义"""
        self.__funcs.append(func)

    def adddata(self, data):
        """增加新数据定义"""
        self.__datas.append(data)

    def adddef(self, d):
        """增加新的宏定义"""
        self.__defs.append(d)

    def build(self):
        """生成文件"""
        # Generate file head.
        now_date = strftime('%Y-%m-%d', localtime(time()))

        cnt = '/**\n'
        cnt += ' *@file\t\t{}\n'.format(op.basename(self.__fpath))
        cnt += ' *@author\t{}\n'.format(self.__author)
        # cnt += ' *@date\t\t{}\n'.format(now_date)
        cnt += ' *@date\t\t{}\n'.format('<auto generate code, not tag date>')
        cnt += ' *@version\t{}\n'.format(self.__ver)
        cnt += ' *\n'
        cnt += ' *@brief\tAuto generate by script tools, do not modify this file!!\n'
        cnt += ' */\n\n'

        basename = op.splitext(op.basename(self.__fpath))[0]
        extension = op.splitext(op.basename(self.__fpath))[1][1:]
        macro = '__PYLLBC_AUTOGEN_{}_{}__'.format(basename.upper(), extension.upper())
        cnt += '#ifndef {}\n'.format(macro)
        cnt += '#define {}\n\n'.format(macro)

        # Generate include stmts
        for incl in self.__incls:
            cnt += '#include "{}"\n'.format(incl)
        if self.__incls:
            cnt += '\n'

        # Generate classes, functions and datas.
        for cls in self.__clses:
            cnt += cls.build()
        for func in self.__funcs:
            cnt += func.build()
        for data in self.__datas:
            cnt += data.build()

        # Generate defines.
        for d in self.__defs:
            cnt += '#define {}\n'.format(d)
        if self.__defs:
            cnt += '\n'

        # End of file.
        cnt += '#endif // !{}\n\n'.format(macro)

        with open(self.__fpath, 'w+') as f:
            f.write(cnt)

