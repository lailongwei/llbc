# -*- coding: utf-8 -*-
"""
pyllbc测试主文件
"""

import sys
from common import *
from core import *
from communication import *
from app import *


def main():
    # set dump file first.
    import llbc
    llbc.inl.HandleCrash('pyllbc_testsuite_dumpfile')

    # #### common part.
    LLBCInfoTest()()
    # StreamTest()()
    # #### core part.
    # GUIDTest()()
    # BundleTest()()
    # RandomTest()()
    # PropertiesTest()()
    # EventTestCase()()
    # LogTest()()

    # #### communication part.
    # TimerTest()()
    # SvcBaseTest()()
    # MultiSvcTest()()
    # MulticastTest()()
    # EventTest()()


if __name__ == '__main__':
    main()
