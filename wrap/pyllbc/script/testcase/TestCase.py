# -*- coding: utf-8 -*-

import llbc

class pyllbcTestCase(object):
    def __init__(self, name=""):
        self.__c_obj = llbc.inl.NewTestCase(name)

    def __del__(self):
        llbc.inl.DelTestCase(self.__c_obj)

    def __call__(self, arg=None):
        return self.run(arg)

    @property
    def name(self):
        return llbc.inl.GetTestCaseName(self.__c_obj)
        
    @name.setter
    def name(self, tcname):
        return llbc.inl.SetTestCaseName(self.__c_obj, tcname)

    def run(self, arg=None):
        """Please rewrite this method"""
        pass

llbc.TestCase = pyllbcTestCase

