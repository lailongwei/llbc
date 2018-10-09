# -*- coding: utf-8 -*-
"""
CPP函数定义,用于生成函数.
"""


from cppvisitattr import CppVisit


class CppFun(object):
    VISIT_NONE = 0
    VISIT_PUBLIC = 1
    VISIT_PROTECTED = 2
    VISIT_PRIVATE = 3

    def __init__(self, name, rtn='void', args=(), visit=CppVisit(CppVisit.NONE)):
        self.__name = name
        self.__rtn = rtn
        self.__args = args
        self.__visit = visit
        self.__stmts = []
        self.__initStmts = []

    def addstmt(self, stmt):
        """添加语句"""
        self.__stmts.append(stmt)

    def addinitstmt(self, stmt):
        """添加初始化列表语句"""
        self.__initStmts.append(stmt)

    def build(self, indent=0):
        """生成函数"""
        beg_indent = ' ' * indent
        stmt_indent = beg_indent + ' ' * 4

        function = self.__visit.build(indent - 4)
        function += beg_indent
        if self.__rtn:
            function += ' ' + self.__rtn + ' '
        function += self.__name
        function += '('
        if isinstance(self.__args, (list, tuple)):
            argslen = len(self.__args)
            for argidx in xrange(0, argslen):
                if argidx != 0:
                    function += ', '
                function += self.__args[argidx]

        function += ')\n'
        if self.__initStmts:
            function += beg_indent + ': {0}\n'.format(self.__initStmts[0])
            for i in range(1, len(self.__initStmts)):
                function += beg_indent + ', {0}\n'.format(self.__initStmts[i])

        function += beg_indent + '{\n'
        for stmt in self.__stmts:
            function += stmt_indent
            function += stmt + '\n'
        function += beg_indent + '}\n\n'

        return function
