# -*- coding: utf-8 -*-

import llbc

def pyllbc_foreach(func, iterable):
    """
    Traverse the iterable value to execute function, mo matter the func return.
    """
    for elem in iterable:
        func(elem)

llbc.foreach = pyllbc_foreach

