# The MIT License(MIT)

# Copyright(c) 2013 lailongwei<lailongwei@126.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files(the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions :
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# -*- coding: utf-8 -*-

import inspect as pyllbcInspect

import llbc

class pyllbcLog(object):
    DEBUG = 0
    INFO = 1
    WARN = 2
    ERROR = 3
    FATAL = 4

    __log_fileinfo = False

    @staticmethod
    def init(cfg_file):
        """Initialize log"""
        llbc.inl.InitLoggerMgr(cfg_file)

    @classmethod
    def enablelogfileinfo(cls):
        """Enable log file name and lineno informations to log file"""
        cls.__log_fileinfo = True

    @classmethod
    def disablelogfileinfo(cls):
        """Disabl log file name and lineno informations to log file"""
        cls.__log_fileinfo = False

    @classmethod
    def d(cls, msg, logger='root', tag=''):
        """Log debug level message"""
        cls.__logmsg(cls.DEBUG, msg, logger, tag)

    @classmethod
    def i(cls, msg, logger='root', tag=''):
        """Log info level message"""
        cls.__logmsg(cls.INFO, msg, logger, tag)

    @classmethod
    def w(cls, msg, logger='root', tag=''):
        """Log warn level message"""
        cls.__logmsg(cls.WARN, msg, logger, tag)

    @classmethod
    def e(cls, msg, logger='root', tag=''):
        """Log error level message"""
        cls.__logmsg(cls.ERROR, msg, logger, tag)

    @classmethod
    def f(cls, msg, logger='root', tag=''):
        """Log fatal level message"""
        cls.__logmsg(cls.FATAL, msg, logger, tag)

    @classmethod
    def __logmsg(cls, lv, msg, logger='root', tag=''):
        """Log specific level message"""
        if cls.__log_fileinfo:
            caller_stack = pyllbcInspect.stack()[2]
            filename = caller_stack[1]
            lineno = caller_stack[2]
        else:
            filename = None
            lineno = 0

        if not isinstance(msg, str):
            if isinstance(msg, unicode):
                msg = msg.encode('utf-8')
            else:
                msg = str(msg)

        llbc.inl.LogMsg(lv, filename, lineno, msg, logger, tag)


llbc.Log = pyllbcLog

