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

