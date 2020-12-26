# -*- coding: utf-8 -*-

import llbc

class pyllbcBaseLibFacade(object):
    """Dynamic library facade base facade"""
    def __init__(self, cobj, name, meths):
        self._c_obj = cobj
        self._name = name
        self._meths = meths

    def __del__(self):
        llbc.Log.d('!!!!!!!! will delete base lib facade, name:{}'.format(self.__class__.__name__))

    @property
    def is_lib_facade(object):
        return True

    @property
    def meths(self):
        return self._meths

    @property
    def facade_name(self):
        """Get facade name"""
        return self._name

    def call(self, meth, arg):
        """
        Call facade methods
        :param arg: the method call argument, now supported type:None/boolean/int/long/float/str/dict.
        :return: the method execute result.
        """
        return llbc.inl.CallFacadeMethod(self._c_obj, meth, arg)

llbc.BaseLibFacade = pyllbcBaseLibFacade
