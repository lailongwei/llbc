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

    def __str__(self):
        return llbc.inl.PyTimerToString(self.__c_obj)

    @property
    def timer_id(self):
        return llbc.inl.PyTimerGetTimerId(self.__c_obj)

    @property
    def first_period(self):
        return llbc.inl.PyTimerGetFirstPeriod(self.__c_obj)

    @property
    def period(self):
        return llbc.inl.PyTimerGetPeriod(self.__c_obj)

    @property
    def is_scheduled(self):
        return llbc.inl.PyTimerIsScheduled(self.__c_obj)

    @property
    def is_handling_timeout(self):
        return llbc.inl.PyTimerIsHandlingTimeout(self.__c_obj)

    @property
    def is_handling_cancel(self):
        return llbc.inl.PyTimerIsHandlingCancel(self.__c_obj)

    @property
    def total_trigger_count(self):
        return llbc.inl.PyTimerGetTotalTriggerCount(self.__c_obj)

    @property
    def triggered_count(self):
        return llbc.inl.PyTimerGetTriggeredCount(self.__c_obj)

    @property
    def ignored_dead_ref(self):
        return llbc.inl.PyTimerIsIgnoredDeadRef(self.__c_obj)

    @ignored_dead_ref.setter
    def ignored_dead_ref(self, flag):
        llbc.inl.PyTimerSetIgnoredDeadRef(self.__c_obj, flag)

    def schedule(self, first_period, period=None, trigger_count=-1):
        """Schedule timer"""
        if period is None:
            period = first_period 

        llbc.inl.PyTimerSchedule(self.__c_obj, first_period, period, trigger_count)

    def schedule2(self, first_time, period=None, fmtstr='%Y-%m-%d %H:%M:%S', trigger_count=-1):
        """
        Schedule timer, arguments is datetime type object, str type object, or numeric type object,
            if first_time type is datetime type object, will use it as expire time.
            if first_time type is str type, will convert to datetime type to use.
            if first_time type is numeric type, will as timestamp to use, as seconds.
        If not specified the tzinfo, llbc will automatic use local tzinfo to fill.
        """
        if isinstance(first_time, unicode):
            first_time = first_time.decode('utf-8')
        if isinstance(first_time, str):
            first_time = _dt.strptime(first_time, fmtstr)

        if isinstance(first_time, _dt):
            ts = _time.mktime(first_time.timetuple()) + first_time.microsecond / 1000000.0
        else:
            ts = first_time

        now = _time.time()
        if ts < now:
            raise llbc.error('first_time[{}] < nowtime[{}], schedule timer failed'.format(first_time, _dt.fromtimestamp(now)))

        self.schedule(int((ts - now) * 1000), int(period * 1000), trigger_count)

    def cancel(self):
        """Cancel timer"""
        llbc.inl.PyTimerCancel(self.__c_obj)

llbc.Timer = pyllbcTimer
