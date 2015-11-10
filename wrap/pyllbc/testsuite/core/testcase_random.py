# -*- coding: utf-8 -*-
"""
random test code.
"""


from time import time


from llbc import TestCase
from llbc import Random


class RandomTest(TestCase):
    def run(self, *args, **kwargs):
        print 'Random test:'
        print 'Default rand():'

        all_output = ''
        for i in range(0, 100):
            if i != 0 and i % 10 == 0:
                all_output += '\n'
            all_output += '{0:13d} '.format(Random.rand())
        print all_output

        print 'rand(100):', Random.rand(100)
        print 'rand(100.0):', Random.rand(100.0)
        print 'rand(None, 100):', Random.rand(None, 100)
        print 'rand(None, 100.0):', Random.rand(None, 100.0)
        print 'rand(100, 200):', Random.rand(100, 200)
        print 'rand(100, 200.0):', Random.rand(100, 200.0)
        print 'rand(200.0, 100):', Random.rand(200.0, 100)
        print 'rand(100.0, 200.0):', Random.rand(100.0, 200.0)
        print 'rand(None, None):', Random.rand(None, None)

        print 'rand53real():', Random.rand53real()

        Random.seed(int(time()))
        print 'after seed(), rand():', Random.rand()

        print 'Press any key to continue ...'
        _ = raw_input()

        return 0