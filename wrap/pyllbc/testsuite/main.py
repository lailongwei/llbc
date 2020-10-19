# -*- coding: utf-8 -*-
"""
pyllbc测试主文件
"""

from common import *
from core import *
from communication import *


def main():
    # set dump file first.
    import llbc
    llbc.inl.SetDumpFile('pyllbc_testsuite_dumpfile')

    # #### common part.
    LLBCInfoTest()()
    # StreamTest()()

    import types
    # #### core part.
    # GUIDTest()()
    # BundleTest()()
    # RandomTest()()
    # PropertyTest()()
    # EventTestCase()()

    # #### communication part.
    # TimerTest()()
    # SvcBaseTest()()
    # MultiSvcTest()()
    # MulticastTest()()
    # EventTest()()


if __name__ == '__main__':
    main()
