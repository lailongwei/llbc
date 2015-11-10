# -*- coding: utf-8 -*-

__author__ = 'Longwei Lai'
__create_date__ = '2014/9/29'
__doc__ = """file description"""

from llbc import Stream, forsend, forrecv, bindto


@forsend(1)
@forrecv(101)
@bindto('server')
class PkgA(object):
    def __init__(self):
        self.val = 0

    def encode(self, s):
        assert isinstance(s, Stream)
        s.packint32(self.val)

        return True

    def decode(self, s):
        assert isinstance(s, Stream)
        self.val = s.unpackint32()

        return True


