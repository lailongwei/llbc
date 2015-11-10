# -*- coding: utf-8 -*-

from os import path as op

import llbc

class pyllbcBundle(object):
    """The Bundle about operations encapsulation"""
    def __init__(self, path, main=False):
        self.__main = main
        if self.__main:
            self.__c_obj = llbc.inl.GetMainBundle()
        else:
            self.__c_obj = llbc.inl.NewBundle()
            llbc.inl.InitBundle(self.__c_obj, path)

    def __del__(self):
        if not self.__main:
            llbc.inl.DelBundle(self.__c_obj)

    @staticmethod
    def mainbundle():
        return pyllbcBundle(path="", main=True)

    @property
    def path(self):
        return llbc.inl.GetBundlePath(self.__c_obj)

    @property
    def name(self):
        return llbc.inl.GetBundleName(self.__c_obj)

    def getrespath(self, resname, ext="", intdir=""):
        return llbc.inl.GetBundleResPath(self.__c_obj, resname, ext, intdir)
        
llbc.Bundle = pyllbcBundle

