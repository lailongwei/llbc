# -*- coding: utf-8 -*-

__author__ = 'Longwei Lai'
__create_date__ = '2014/9/29'
__doc__ = """file description"""

from llbc import handler, bindto


@handler(2, 102)
@bindto('client', 'client2')
class HandlerB(object):
    def __init__(self):
        print 'Handler B created'

    def __call__(self, packet):
        print 'handle packet: {}'.format(packet)





