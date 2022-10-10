# -*- coding: utf-8 -*-
"""
Stream测试代码
"""

from time import time
from llbc import TestCase, Stream


class PackableObjA(object):
    def __init__(self):
        self.int_val = 3
        self.bool_val = True
        self.str_val = 'hello world'
        self.uni_val = u'{unicode} hello world'

    def __str__(self):
        return 'int_val: {}, bool_val: {}, str_val: {}, uni_val: {}'\
            .format(self.int_val, self.bool_val, self.str_val, self.uni_val)

    def encode(self, s):
        assert isinstance(s, Stream)
        s.pack('iBSU', self.int_val, self.bool_val, self.str_val, self.uni_val)

        return True

    def decode(self, s):
        assert isinstance(s, Stream)
        self.int_val, self.bool_val, self.str_val, self.uni_val = s.unpack('iBSU')

        return True


class StreamTest(TestCase):
    def run(self, arg):
        print 'llbc Stream test:'

        self._basetest()
        self._brief_meth_call_test()
        self._obj_pack_test()
        self._composite_test()
        self._perf_test()

    @staticmethod
    def _basetest():
        print 'Base test:'

        s = Stream(128)

        print 'Pack/Unpack base data type:'
        # byte, char, bool, int16, int16, int32, int32, int64, int64, float, double, str
        will_pack = ('s', 'c', True, 16, -16, 32, -32, 64, -64, 1.3, 2.6, 'hello world')
        print 'pack() test, will pack data: {}'.format(will_pack)

        fmt = 'bbBssiiqqfdS'
        s.pack(fmt, *will_pack)
        print 'after pack, pos: {}'.format(s.pos)
        print 'raw data: {}'.format(s)

        s.pos = 0
        unpacked = s.unpack(fmt)
        print 'unpacked, data: {}'.format(unpacked)
        print

        print 'Pack/Unpack list:'
        s.pos = 0
        will_pack = ['hello', 'world']
        print 'Before pack: {}'.format(will_pack)
        s.pack('[S]', will_pack)

        s.pos = 0
        unpacked = s.unpack('[S]')
        print 'After unpack list: {}'.format(unpacked)
        print

        print 'Pack/Unpack tuple:'
        s.pos = 0
        will_pack = ('hello', 'world')
        print 'Before pack: {}'.format(will_pack)
        s.pack('(S)', will_pack)

        s.pos = 0
        unpacked = s.unpack('[S]')
        print 'After unpack tuple: {}'.format(unpacked)
        print

        print 'Pack/Unpack dict:'
        s.pos = 0
        will_pack = {'name': 'Judy', 'age': '18', 'home': 'California'}
        print 'Before pack: {}'.format(will_pack)
        s.pack('{S:S}', will_pack)

        s.pos = 0
        unpacked = s.unpack('{S:S}')
        print 'After unpack dict: {}'.format(unpacked)
        print

    def _brief_meth_call_test(self):
        print 'Brief method call test:'

        s = Stream()
        test = self._pack_unpack_raw_test
        test('c', (s.packbyte, s.unpackbyte), 'Byte')
        test((True, False), (s.packbool, s.unpackbool), 'Bool')
        test((16, -16), (s.packint16, s.unpackint16), 'Int16')
        test((32, -32), (s.packint32, s.unpackint32), 'Int32')
        test((64, -64), (s.packint64, s.unpackint64), 'Int64')
        test(1.3, (s.packfloat, s.unpackfloat), 'Float')
        test(2.6, (s.packdouble, s.unpackdouble), 'Double')
        test(('hello', 'world'), (s.packstr, s.unpackstr), 'Str')
        test((u'unicode_hello', u'unicode_world'), (s.packunicode, s.unpackunicode), 'Unicode')

        # Special, pack/unpack class test.
        will_pack = 3
        print 'Pack/Unpack class:'
        s.pos = 0
        s.packobj(3)

        s.pos = 0
        unpacked = s.unpackcls(int)
        print '{} <--> {}'.format(will_pack, unpacked)
        print

    def _pack_unpack_raw_test(self, val, meths, prompt_str):
        if isinstance(val, (tuple, list)):
            for v in val:
                self._pack_unpack_raw_test(v, meths, prompt_str)
            return

        s = meths[0].__self__
        print 'Pack/Unpack {}:'.format(prompt_str)

        s.pos = 0
        meths[0](val)
        print ' packed, pos:{}'.format(s.pos)

        s.pos = 0
        unpacked = meths[1]()
        print ' unpacked, pos:{}'.format(s.pos)
        print '{} <--> {}'.format(val, unpacked)

    def _obj_pack_test(self):
        print 'Obj/Class pack test:'

        s = Stream()
        obj = PackableObjA()
        print 'Before pack obj: {}'.format(obj)
        s.pack('C<PackableObjA>', obj)

        print 'After pack object, size: {}'.format(s.pos)

        s.pos = 0
        unpacked = s.unpack('C<PackableObjA>')[0]
        print 'After unpack obj: {}'.format(unpacked)
        print

    def _composite_test(self):
        print 'Composite test:'

        s = Stream()
        will_pack = {1: {'hello': ['Hello', True, None, 3], 'world': ['World', False, None, -3]}}
        print 'Will pack data: {}'.format(will_pack)

        s.pack('{i:{S:[SBNC<int>]}}', will_pack)
        print 'Pack done, size: {}'.format(s.pos)

        s.pos = 0
        unpacked = s.unpack('{i:{S:[SBNC<int>]}}')
        print 'After unpack data: {}'.format(unpacked)
        print

    def _perf_test(self):
        print 'Performance test:'

        s = Stream()
        will_pack = {1: {'hello': ['Hello', True, None, 3], 'world': ['World', False, None, -3]}}
        print 'Will pack data: {}'.format(will_pack)

        beg = time()
        print 'Pack/Unpack times: 1000000'
        for i in range(1000000):
            s.pos = 0
            s.pack('{i:{S:[SBNC<int>]}}', will_pack)

            s.pos = 0
            unpacked = s.unpack('{i:{S:[SBNC<int>]}}')

        elapsed = time() - beg
        print 'Done, used time: {}'.format(elapsed)

