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

        random = Random()
        all_output = ''
        for i in range(0, 100):
            if i != 0 and i % 10 == 0:
                all_output += '\n'
            all_output += '{0:2d} '.format(random.rand())
        print all_output

        print 'rand(100):', random.rand(100)
        print 'rand(None, 100):', random.rand(None, 100)
        print 'rand(None, 100.0):', random.rand(None, 100.0)
        print 'rand(100, 200):', random.rand(100, 200)
        print 'rand(100, 200.0):', random.rand(100, 200.0)
        print 'rand(200.0, 100):', random.rand(200.0, 100)
        print 'rand(100.0, 200.0):', random.rand(100.0, 200.0)
        print 'rand(None, None):', random.rand(None, None)

        print 'randreal():', random.randreal()

        random.seed(int(time()))
        print 'after seed(), rand():', random.rand()

        l = [1, 3, 5, 7, 9, 100, 300, 500, 700, 900, 999]
        print 'choice list: {}: {}'.format(l, random.choice(l))

        print 'Press any key to continue ...'
        _ = raw_input()

        return 0