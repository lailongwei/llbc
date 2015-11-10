# -*- coding: utf-8 -*-
from llbc import facade, bindto

__author__ = 'Longwei Lai'
__create_date__ = '2014/9/29'
__doc__ = """file description"""


@facade
@bindto('server')
class FacadeA(object):
    def __init__(self):
        pass

    def oninitialize(self, d):
        print 'ModuleA init!'

    def ondestroy(self, d):
        print 'ModuleA destroy'
