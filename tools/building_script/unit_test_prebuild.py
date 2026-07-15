# -*- coding: utf-8 -*-
"""
unit_test项目预编译脚本, 用于:
- 完成 gtest/gmock 测试框架编译
"""

from com.log import Log
from googletest_compiler import GoogleTestCompiler

def main():
    # Log 开始 prebild.
    Log.i('Begin prebuild unit_test project')

    # 编译 gtest.
    GoogleTestCompiler().compile()

    Log.i('unit_test prebuild success!')

if __name__ == '__main__':
    main()