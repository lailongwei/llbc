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

