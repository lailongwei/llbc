# -*- coding: utf-8 -*-
"""
简易日志封装
"""

import sys
import datetime


class Log(object):
    """简易日志封装"""
    Debug = 0
    Info = 1
    Warn = 2
    Error = 3

    _log_lv_str = {
        Debug: 'DEBUG',
        Info: 'INFO ',
        Warn: 'INFO ',
        Error: 'ERROR',
    }

    @classmethod
    def d(cls, msg):
        cls._log(cls.Debug, msg)

    @classmethod
    def i(cls, msg):
        cls._log(cls.Info, msg)

    @classmethod
    def w(cls, msg):
        cls._log(cls.Warn, msg)

    @classmethod
    def e(cls, msg):
        cls._log(cls.Error, msg)

    @classmethod
    def fd(cls, msg, *args):
        cls._log(cls.Debug, msg.format(*args))

    @classmethod
    def fi(cls, msg, *args):
        cls._log(cls.Info, msg.format(*args))

    @classmethod
    def fw(cls, msg, *args):
        cls._log(cls.Warn, msg.format(*args))

    @classmethod
    def fe(cls, msg, *args):
        cls._log(cls.Error, msg.format(*args))

    @classmethod
    def _log(cls, lv, msg):
        f = sys.stderr if lv >= cls.Warn else sys.stdout
        f.write('{} [{}] {}\n'.format(datetime.datetime.now().strftime('%m-%d %H:%M:%S'),
                                      cls._log_lv_str[lv],
                                      msg))
        if lv < cls.Warn:
            f.flush()

        if lv >= cls.Error:
            raise Exception(msg)
