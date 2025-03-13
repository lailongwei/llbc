# -*- coding: utf-8 -*-

import types as pyllbcTypes
import inspect as pyllbcInspect

import llbc

class pyllbcLog(object):
    TRACE = 0
    DEBUG = 1
    INFO = 2
    WARN = 3
    ERROR = 4
    FATAL = 5

    __inited = False
    __log_fileinfo = False

    @classmethod
    def init(cls, cfg_file):
        """Initialize log"""
        if cls.__inited:
            raise Exception('Repeatly repeat Log')

        llbc.inl.InitLoggerMgr(cfg_file)
        cls.__inited = True

    @classmethod
    def uninit(cls):
        """Finalize log"""
        if not cls.__inited:
            return

        llbc.inl.UnInitLoggerMgr()
        cls.__inited = False

    @classmethod
    def enablelogfileinfo(cls):
        """Enable log file name and lineno informations to log file"""
        cls.__log_fileinfo = True

    @classmethod
    def disablelogfileinfo(cls):
        """Disabl log file name and lineno informations to log file"""
        cls.__log_fileinfo = False

    @classmethod
    def t(cls, msg, logger='root', tag=''):
        """Log trace level message"""
        cls.__logmsg(cls.TRACE, msg, logger, tag)

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
            func = caller_stack[3]
        else:
            filename = None
            lineno = 0
            func = None

        if not isinstance(msg, str):
            if isinstance(msg, unicode):
                msg = msg.encode('utf-8')
            else:
                msg = str(msg)
        
        # normalize tag
        if isinstance(tag, pyllbcTypes.MethodType):  # method type(included bound/unbound)
            tag = '{}.{}'.format (tag.im_class.__name__, tag.__name__)
        if hasattr(tag, '__name__'): # functions, builtin functions, class/object/type
            tag = tag.__name__
        elif isinstance(tag, unicode):  # unicode type
            tag = tag.encode('utf-8')
        elif not isinstance(tag, str):  # other type(not str)
            tag = str(tag)

        if not cls.__inited:
            print('[Log] {}'.format(msg))
            return

        llbc.inl.LogMsg(lv, filename, lineno, func, msg, logger, tag)


llbc.Log = pyllbcLog

