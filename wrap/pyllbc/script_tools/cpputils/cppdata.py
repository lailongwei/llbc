# -*- coding: utf-8 -*-
"""
CPP数据结构定义
"""


from cppvisitattr import CppVisit


class CppData(object):
    def __init__(self, ty, name, ptr=False, visit=CppVisit(CppVisit.NONE)):
        self.__type = ty
        self.__name = name
        self.__ptr = ptr
        self.__visit = visit

    def build(self, indent=0):
        s = self.__visit.build(indent - 4)

        s += ' ' * indent + self.__type
        if self.__ptr:
            s += ' *'

        s += ' ' + self.__name
        s += ';\n'

        return s