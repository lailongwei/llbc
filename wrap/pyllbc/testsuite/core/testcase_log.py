# -*- coding: utf-8 -*-
"""
log test code.
"""


from llbc import Log
from llbc import TestCase

class LogTest(TestCase):
    def run(self, *args, **kwargs):
        print 'Log test:'

        # Init log.
        Log.init('LogTestCfg.cfg')

        # Enable log fileinfo.
        Log.enablelogfileinfo()

        # base log test.
        self._base_log_test()

        # UnInit log.
        Log.uninit()

    def _base_log_test(self):
        """bae log functions test"""
        Log.t('This is a trace message')
        Log.d('This is a debug message')
        Log.i('This is a info message')
        Log.w('This is a warn message')
        Log.e('This is a error message')
        Log.f('This is a fatal message')
