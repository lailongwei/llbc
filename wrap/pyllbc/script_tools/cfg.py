# -*- coding: utf-8 -*-
"""
配置相关
"""


import sys
from os import path as op

# 库版本字串
LIB_VER = '1.0'
# 库作者描述
LIB_AUTHOR = 'Longwei Lai<lailongwei@126.com>'
# 文件系统编码
SYS_ENC = sys.getfilesystemencoding()
# pyllbc项目路径
PROJ_PATH = op.dirname(op.dirname(__file__))


class Cfg(object):
    @staticmethod
    def getver():
        return LIB_VER

    @staticmethod
    def getauthor():
        return LIB_AUTHOR

    @staticmethod
    def getprojpath():
        return PROJ_PATH

    @staticmethod
    def getscriptpath():
        return op.join(PROJ_PATH, 'script')

    @staticmethod
    def getcodepath():
        return op.join(PROJ_PATH, 'src')
