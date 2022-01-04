# -*- coding: utf-8 -*-
from llbc import facade, bindto

__author__ = 'Longwei Lai'
__create_date__ = '2014/9/29'
__doc__ = """file description"""


@facade
@bindto('client', 'client2')
class FacadeB(object):
    def __init__(self):
        pass

    def oninitialize(self, ev):
        print 'ModuleB init in service: {}'.format(ev.svc.name)

    def ondestroy(self, ev):
        print 'ModuleB destroy in service: {}'.format(ev.svc.name)




