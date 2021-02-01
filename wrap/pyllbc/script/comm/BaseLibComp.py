# -*- coding: utf-8 -*-

import llbc

class pyllbcBaseLibComp(object):
    """
    Dynamic library component base class encapsulation.
    Note:
    - If you want to wrap your loaded dynamic library, please inherit this base class.
    - pyllbc library auto generate component class define if not specific custom component class(internal inherit this base class).
    """
    def __init__(self, cobj, name, meths):
        self._c_obj = cobj
        self._name = name
        self._meths = meths

    def __del__(self):
        pass

    @property
    def is_lib_comp(object):
        return True

    @property
    def meths(self):
        return self._meths

    @property
    def comp_name(self):
        """Get component name"""
        return self._name

    def call(self, meth, arg):
        """
        Call component method.
        :param arg: the method call argument, now supported type:None/boolean/int/long/float/str/dict.
        :return: the method execute result.
        """
        return llbc.inl.CallComponentMethod(self._c_obj, meth, arg)

llbc.BaseLibComp = pyllbcBaseLibComp
