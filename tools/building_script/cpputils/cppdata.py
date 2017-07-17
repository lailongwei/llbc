# -*- coding: utf-8 -*-
"""
CPP数据结构定义
"""


from cppvisitattr import CppVisit


class CppData(object):
    def __init__(self, ty, name, ptr=False, static=False, visit=CppVisit(CppVisit.NONE), init=None):
        self.__type = ty
        self.__name = name
        self.__ptr = ptr
        self.__static = static
        self.__visit = visit
        self.__init = init

    def build(self, indent=0):
        s = self.__visit.build(indent - 4)

        s += ' ' * indent

        if self.__static:
            s += 'static '
        s += self.__type
        if self.__ptr:
            s += ' *'

        s += ' ' + self.__name
        if self.__init:
            s += ' = {0}'.format(self.__init)
        s += ';\n'

        return s
