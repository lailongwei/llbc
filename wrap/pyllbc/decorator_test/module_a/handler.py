# -*- coding: utf-8 -*-

__author__ = 'Longwei Lai'
__create_date__ = '2014/9/29'
__doc__ = """file description"""

from llbc import handler, bindto


@bindto('server')
@handler(1, 101)
class HandlerA(object):
    def __init__(self):
        print 'Handler A created'

    def __call__(self, packet):
        print 'HandlerA handle packet: {}'.format(packet)

