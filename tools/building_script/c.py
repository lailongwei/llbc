# -*- coding: utf-8 -*-
"""
脚本工具配置
"""

import os
import sys
from os import path as op

from defs import ProjType, LangType

# 库版本字串
LIB_VER = os.getenv('LLBC_LIB_VER', '1.0')
# 库作者描述
LIB_AUTHOR = os.getenv('LLBC_LIB_AUTHOR', 'Longwei Lai<lailongwei@126.com>')
# 文件系统编码
SYS_ENC = sys.getfilesystemencoding()
# 脚本目录
SCRIPT_TOOL_PATH = os.getenv('LLBC_SCRIPT_TOOL_PATH', op.dirname(op.abspath(__file__)))
# LICENSE文件目录
LICENSE_FILE_PATH = op.join(op.dirname(op.dirname(SCRIPT_TOOL_PATH)), 'LICENSE')
#LICENSE文件内容
LICENSE_CONTENT = open(LICENSE_FILE_PATH, 'r').readlines()
# 项目名, 目录
PROJ_NAME = os.getenv('LLBC_PROJ_NAME', sys.argv[1])
PROJ_PATH = op.join(op.dirname(op.dirname(SCRIPT_TOOL_PATH)), 'wrap', PROJ_NAME)


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
    def getprojname():
        return PROJ_NAME

    @staticmethod
    def getscriptpath():
        return op.join(PROJ_PATH, 'script')

    @staticmethod
    def getcodepath():
        return op.join(PROJ_PATH, 'src')

    @staticmethod
    def getlangtype():
        if PROJ_NAME == ProjType.pyllbc:
            return LangType.python
        elif PROJ_NAME == ProjType.lullbc:
            return LangType.lua
        else:
            return LangType.unknown

    @classmethod
    def getlicensehead(cls, lang_type=None):
        lang_type = lang_type or cls.getlangtype()
        if lang_type == LangType.python:
            line_prefix = '# '
        elif lang_type == LangType.lua:
            line_prefix = '-- '
        elif lang_type == LangType.cplusplus:
            line_prefix = '// '
        else:
            line_prefix = ''

        cnt = list(LICENSE_CONTENT)
        for i in range(len(cnt)):
            if i == 1:
                continue
            cnt[i] = line_prefix + cnt[i]
        return ''.join(cnt)

    __Dummy = None

