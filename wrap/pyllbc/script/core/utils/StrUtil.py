# -*- coding: utf-8 -*-

import llbc

class pyllbcStrUtil(object):
    """
    pyllbc string util class encapcsulation.
    """

    __native_hash_meth = llbc.inl.Hash
    @classmethod
    def hash(cls, s):
        """
        hash string value
        """
        return cls.__native_hash_meth(s)

llbc.StrUtil = pyllbcStrUtil