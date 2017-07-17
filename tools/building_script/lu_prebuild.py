# -*- coding: utf-8 -*-
"""
@file   lu_prebuild.py
@author lailongwei
@date   2017/7/14
@brief:
        
"""
import sys

from c import Cfg
from lu_script_integrator import LuScriptIntegrator
from native_method_collector import LuNativeMethodCollector as LuNMC


def main():
    # 收集所有lua native methods并进行整合
    code_path = Cfg.getcodepath()
    LuNMC(code_path, classname_base='', filename_base=Cfg.getprojname().capitalize()).build()

    # 读取当前编译是否处于debug模式
    debug = not (len(sys.argv) < 3 or sys.argv[2].upper() != 'DEBUG')
    print 'Build in {0} mode...'.format(debug and 'Debug' or 'Release')

    # 整合所有lua脚本
    print 'Integrate lua scripts...'
    LuScriptIntegrator(debug).build()

    print 'Done!'

if __name__ == '__main__':
    main()
