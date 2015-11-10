# -*- coding: utf-8 -*-

import llbc

def pyllbcEnum(*sequential, **named):
    """
    The enum helper function.
    """
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)

llbc.Enum = pyllbcEnum

