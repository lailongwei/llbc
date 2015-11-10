# -*- coding: utf-8 -*-
"""
"""


from llbc import TestCase
from llbc import GUID


class GUIDTest(TestCase):
    def run(self, arg):
        print 'GUID test case:'
        for i in range(0, 10):
            print 'generate guid:', GUID.generate()

        print 'Press any key to continue...'
        _ = raw_input()

        return 0
