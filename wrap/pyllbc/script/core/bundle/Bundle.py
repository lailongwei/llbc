# The MIT License(MIT)

# Copyright(c) 2013 lailongwei<lailongwei@126.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files(the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions :
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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

