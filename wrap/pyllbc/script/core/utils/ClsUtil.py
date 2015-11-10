# -*- coding: utf-8 -*-

import llbc

def pyllbc_ischild(child_cls, base_cls):
    if child_cls is base_cls:
        return True
    
    for cls in child_cls.__bases__:
        if pyllbc_ischild(cls, base_cls):
            return True
    return False

llbc.ischild = pyllbc_ischild

