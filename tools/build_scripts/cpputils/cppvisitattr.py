# -*- coding: utf-8 -*-
"""
Cpp类访问属性封装
"""


class CppVisit(object):
    NONE = 0
    PUBLIC = 1
    PROTECTED = 2
    PRIVATE = 3

    def __init__(self, visit):
        if visit not in range(CppVisit.NONE, CppVisit.PRIVATE + 1):
            raise ValueError('visit type not in legal range')

        self.__visit = visit

    def build(self, indent=0):
        """生成访问控制字符串"""
        clazz = CppVisit

        visit = self.__visit
        if visit == clazz.NONE:
            return ""

        s = ' ' * (0 if indent <= 0 else indent)
        if visit == clazz.PUBLIC:
            s += "public"
        elif visit == clazz.PROTECTED:
            s += 'protected'
        elif visit == clazz.PRIVATE:
            s += 'private'

        return s + ':\n'
