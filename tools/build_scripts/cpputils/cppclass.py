# -*- coding: utf-8 -*-
"""
CPP类封装
"""


class CppClass(object):
    def __init__(self, name, constraint='', indent=0):
        self.__name = name
        self.__constraint = constraint
        self.__indent = indent

        self.__datas = []
        self.__methods = []

    def adddata(self, data):
        """增加数据成员"""
        self.__datas.append(data)

    def addmethod(self, method):
        """增加类方法"""
        self.__methods.append(method)

    def build(self):
        """生成类定义"""
        indent = self.__indent
        beg_indent = ' ' * indent

        if not self.__constraint:
            c = beg_indent + 'class {0}\n'.format(self.__name)
        else:
            c = beg_indent + 'class {0} {1}\n'.format(self.__constraint, self.__name)

        c += beg_indent + '{\n'
        for method in self.__methods:
            c += method.build(indent + 4)

        for data in self.__datas:
            c += data.build(indent + 4)

        c += beg_indent + '};\n\n'

        return c
