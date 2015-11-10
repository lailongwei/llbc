# -*- coding: utf-8 -*-
"""
pyllbc测试主文件
"""

from common import *
from core import *
from communication import *


def main():
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
    # CustomHeaderPartsTest()()


if __name__ == '__main__':
    main()
