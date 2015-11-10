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

