# -*- coding: utf-8 -*-

import llbc
from llbc import TestCase

class LLBCInfoTest(TestCase):
    def run(self, arg):
        print 'llbc information:'

        print 'info(non verbose):', llbc.GetVersionInfo(False)
        print 'info(verbose):'
        print llbc.GetVersionInfo(True)

        print 'major version:', llbc.MajorVersion
        print 'minor version:', llbc.MinorVersion
        print 'update num:', llbc.UpdateNo
        print 'is debug version:', llbc.Debug

        print 'machine endian:', llbc.Endian.MachineEndian

        print 'Press any key to continue...'
        _ = raw_input()

        return 0
