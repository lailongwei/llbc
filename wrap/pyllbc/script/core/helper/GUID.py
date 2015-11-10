# -*- coding: utf-8 -*-

import llbc

class pyllbcGUID(object):
    """The GUID about operations encapsulation"""
    def __init__(self):
        pass

    @classmethod
    def generate(clazz):
        return llbc.inl.GenGUID()

llbc.GUID = pyllbcGUID

