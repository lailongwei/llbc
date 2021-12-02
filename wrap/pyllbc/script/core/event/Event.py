# -*- coding: utf-8 -*-

import llbc

class pyllbcEvent(object):
    """Event object encapsulation"""
    def __init__(self, evid, cobj=0):
        self._evid = evid
        if cobj != 0:
            self._cobj = cobj
        else:
            self._cobj = llbc.inl.Event_New(self, evid)

    def __del__(self):
        llbc.inl.Event_Del(self._cobj)

    @property
    def evid(self):
        return self._evid
    @evid.setter
    def evid(self, value):
        self._evid = value

    @property
    def cobj(self):
        return self._cobj
    @cobj.setter
    def cobj(self, value):
        self._cobj = value 

    def __getitem__(self, key):
        try:
            return llbc.inl.Event_GetItem(self._cobj, key)
        except IndexError, e:
            return super(pyllbcEvent, self).__getitem__(key)

    def __setitem__(self, key, value):
        llbc.inl.Event_SetItem(self._cobj, key, value)

    def __str__(self):
        return 'Event[id:{}, cobj:0x{:x}]'.format(self._evid, self._cobj)

llbc.Event = pyllbcEvent
