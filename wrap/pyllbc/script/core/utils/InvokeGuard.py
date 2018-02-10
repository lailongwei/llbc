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

class pyllbcInvokeGuard(object):
    """
    pyllbc library Invoke-Guard class encapsulation
    """
    def __init__(self, cbl=None, *args, **kwargs):
        self._cbl = cbl
        self._args, self._kwargs = args, kwargs
    
    def cancel(self):
        """
        Cancel this guard
        """
        self._cbl = None
        self._args, self._kwargs = None, None

    def __del__(self):
        self._cbl(*self._args, **self._kwargs)

llbc.InvokeGuard = pyllbcInvokeGuard

