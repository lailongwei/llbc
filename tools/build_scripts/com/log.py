# -*- coding: utf-8 -*-
"""
简易日志封装
"""

import sys
import datetime


class Log(object):
    """简易日志封装"""
    Trace = 0
    Debug = 1
    Info = 2
    Warn = 3
    Error = 4

    _log_lv_str = {
        Trace: 'TRACE',
        Debug: 'DEBUG',
        Info: 'INFO',
        Warn: 'WARN',
        Error: 'ERROR',
    }

    @classmethod
    def t(cls, msg):
        cls._log(cls.Trace, msg)

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
    def ft(cls, msg, *args):
        cls._log(cls.Trace, msg.format(*args))

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

        log_prefix = ''
        log_suffix = ''
        is_win = sys.platform.lower().startswith('win')
        if not is_win and lv >= cls.Info:
            log_prefix = '\033['
            if lv == cls.Info:
                log_prefix += '32m'
            elif lv == cls.Warn:
                log_prefix += '33m'
            elif lv == cls.Error:
                log_prefix += '31m'

            log_suffix = '\033[0m'

        f.write('{}{} [{:<5}]<llbc> {}{}\n'.format(log_prefix,
                                                   datetime.datetime.now().strftime('%y-%m-%d %H:%M:%S'),
                                                   cls._log_lv_str[lv],
                                                   msg,
                                                   log_suffix))
        if lv < cls.Warn:
            f.flush()

        if lv >= cls.Error:
            raise Exception(msg)
