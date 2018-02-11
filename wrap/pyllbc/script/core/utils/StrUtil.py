# -*- coding: utf-8 -*-

import llbc

class pyllbcStrUtil(object):
    """
    pyllbc string util class encapcsulation.
    """

    __native_hash_meth = llbc.inl.HashString
    @classmethod
    def hash_string(cls, s):
        """
        hash string value
        """
        return cls.__native_hash_meth(s)

llbc.StrUtil = pyllbcStrUtil