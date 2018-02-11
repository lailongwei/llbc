# -*- coding: utf-8 -*-

import time as _time

from datetime import datetime as _dt
from datetime import tzinfo as _tzinfo
from datetime import timedelta as _timedelta

import llbc

class _pyllbcGMT(_tzinfo):
    """llbc library GMT tzinfo class encapsulation"""
    _delta = _timedelta(0)
    def utcoffset(self, dt):
        return self._delta

    def tzname(self):
        return "GMT+0"

    def dst(self, dt):
        return None

class _pyllbcLOCAL(_tzinfo):
    """llbc library LOCAL tzinfo class encapsulation"""
    _delta = _timedelta(seconds=-_time.timezone)
    _tzname = 'GMT +{}'.format(-_time.timezone / 3600) if _time.timezone < 0 else \
            ('GMT -{}'.format(_time.timezone / 3600) if _time.timezone > 0 else 'GMT +0')

    def utcoffset(self, dt):
        return self._delta

    def tzname(self):
        return self._tzname

    def dst(self, dt):
        return None

class pyllbcTimer(object):
    """llbc library timer class encapsulation"""
    INVALID_TIMER_ID = 0

    """llbc library timer class encapsulation"""
    def __init__(self, ontimeout, oncancel=None):
        if not callable(ontimeout):
            raise TypeError("'ontimeout' obj must callable")

        if not callable(oncancel):
            self.__c_obj = llbc.inl.NewPyTimer(self, ontimeout)
        else:
            self.__c_obj = llbc.inl.NewPyTimer(self, ontimeout, oncancel)

    def __del__(self):
        llbc.inl.DelPyTimer(self.__c_obj)

    @property
    def timerid(self):
        return llbc.inl.PyTimerGetTimerId(self.__c_obj)

    @property
    def duetime(self):
        return llbc.inl.PyTimerGetDueTime(self.__c_obj)

    @property
    def period(self):
        return llbc.inl.PyTimerGetPeriod(self.__c_obj)

    @property
    def isscheduling(self):
        return llbc.inl.PyTimerIsScheduling(self.__c_obj)

    @property
    def istimeouting(self):
        return llbc.inl.PyTimerIsTimeouting(self.__c_obj)

    @property
    def iscancelling(self):
        return llbc.inl.PyTimerIsCancelling(self.__c_obj)

    @property
    def ignored_dead_ref(self):
        return llbc.inl.PyTimerIsIgnoredDeadRef(self.__c_obj)

    @ignored_dead_ref.setter
    def ignored_dead_ref(self, flag):
        llbc.inl.PyTimerSetIgnoredDeadRef(self.__c_obj, flag)

    def schedule(self, duetime, period=None):
        """Schedule timer"""
        if period is None:
            period = duetime

        llbc.inl.PyTimerSchedule(self.__c_obj, duetime, period)

    def schedule2(self, duetime, period, fmtstr='%Y-%m-%d %H:%M:%S'):
        """
        Schedule timer, arguments is datetime type object, str type object, or numeric type object,
            if duetime type is datetime type object, will use it as expire time.
            if duetime type is str type, will convert to datetime type to use.
            if duetime type is numeric type, will as timestamp to use, as seconds.
        If not specified the tzinfo, llbc will automatic use local tzinfo to fill.
        """
        if isinstance(duetime, unicode):
            duetime = duetime.decode('utf-8')
        if isinstance(duetime, str):
            duetime = _dt.strptime(duetime, fmtstr)

        if isinstance(duetime, _dt):
            ts = _time.mktime(duetime.timetuple()) + duetime.microsecond / 1000000.0
        else:
            ts = duetime

        now = _time.time()
        if ts < now:
            raise llbc.error('duetime[{}] < nowtime[{}], schedule timer failed'.format(duetime, _dt.fromtimestamp(now)))

        self.schedule(int((ts - now) * 1000), int(period * 1000))

    def cancel(self):
        """Cancel timer"""
        llbc.inl.PyTimerCancel(self.__c_obj)

llbc.Timer = pyllbcTimer
