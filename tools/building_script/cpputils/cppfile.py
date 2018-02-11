# -*- coding: utf-8 -*-
"""
CPP文件简单封装,用于生成cpp文件.
"""

from os import path as op
from time import time, localtime, strftime


class CppFile(object):
    def __init__(self, fpath, author='Unknown author', ver='1.0', doc='', include_macro_prefix=None):
        self.__fpath = fpath
        self.__author = author
        self.__ver = ver
        self.__doc = doc
        self.__include_macro_prefix = include_macro_prefix
        self.__custom_filehead = None

        self.__incls = []
        self.__clses = []
        self.__funcs = []
        self.__datas = []
        self.__defs = []
        self.__codelines = []

    # region Properties fpath, author, version, doc, include_macro_prefix
    @property
    def fpath(self):
        """返回文件路径"""
        return self.__fpath

    @property
    def author(self):
        """返回文件作者"""
        return self.__author
    @author.setter
    def author(self, author):
        """设置文件作者"""
        self.__author = author

    @property
    def version(self):
        """返回文件版本"""
        return self.__ver
    @version.setter
    def version(self, version):
        """设置文件版本"""
        self.__ver = version

    @property
    def doc(self):
        """取得文件描述文档"""
        return self.__doc
    @doc.setter
    def doc(self, doc):
        """设置文件描述文档"""
        self.__doc = doc

    @property
    def custom_filehead(self):
        """取得自定义文件头"""
        return self.__custom_filehead
    @custom_filehead.setter
    def custom_filehead(self, custom_filehead):
        """设置自定义文件头"""
        self.__custom_filehead = custom_filehead

    @property
    def include_macro_prefix(self):
        """取得文件头包含宏前缀"""
        return self.__include_macro_prefix
    @include_macro_prefix.setter
    def include_macro_prefix(self, prefix):
        """设置取得文件头包含宏前缀"""
        self.__include_macro_prefix = prefix
    # endregion

    # region 文件操作: 增加include语句, 增加类, 增加函数, 增加新数据定义, 增加宏定义
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

    def addcodeline(self, line):
        """增加代码行"""
        self.__codelines.append(line)
    # endregion

    # region 构建
    def build(self, sort_incl=True):
        """生成文件"""
        # Generate file head.
        now_date = strftime('%Y-%m-%d', localtime(time()))

        if not self.__custom_filehead:
            cnt = '/**\n'
            cnt += ' *@file\t\t{0}\n'.format(op.basename(self.__fpath))
            cnt += ' *@author\t{0}\n'.format(self.__author)
            cnt += ' *@date\t\t{0}\n'.format('<auto generate code, not tag date>')
            cnt += ' *@version\t{0}\n'.format(self.__ver)
            cnt += ' *\n'
            cnt += ' *@brief\tAuto generate by script tools, do not modify this file!!\n'
            cnt += ' *@doc\t{0}\n'.format(self.__doc)
            cnt += ' */\n\n'
        else:
            cnt = self.__custom_filehead
            cnt += '\n'

        basename = op.splitext(op.basename(self.__fpath))[0]
        parent_dirname = op.basename(op.dirname(self.__fpath)).upper()
        extension = op.splitext(op.basename(self.__fpath))[1][1:]
        if extension in ('h', 'hpp'):
            macro = '__{0}_AUTOGEN_{1}_{2}_{3}__'.format(self.include_macro_prefix or 'NOPREFIX',
                                                         parent_dirname, basename.upper(), extension.upper())
            cnt += '#ifndef {0}\n'.format(macro)
            cnt += '#define {0}\n\n'.format(macro)
        else:
            macro = ''

        # Generate include stmts
        incls = self.__incls
        if sort_incl:
            incls = sorted(incls)
        for incl in incls:
            cnt += '#include "{0}"\n'.format(incl)
        if incls:
            cnt += '\n'

        # Generate classes, functions and datas.
        for cls in self.__clses:
            cnt += cls.build()
        for func in self.__funcs:
            cnt += func.build()
        for data in self.__datas:
            cnt += data.build()

        # Generate codelines.
        for codeline in self.__codelines:
            cnt += codeline + ';\n'

        # Generate defines.
        for d in self.__defs:
            cnt += '#define {0}\n'.format(d)
        if self.__defs:
            cnt += '\n'

        # End of file.
        if macro:
            cnt += '#endif // !{0}\n\n'.format(macro)

        with open(self.__fpath, 'wb+') as f:
            f.write(cnt)
    # endregion

    __Dummy = None
