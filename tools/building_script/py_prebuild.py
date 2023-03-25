# -*- coding: utf-8 -*-
"""
pyllbc项目预编译脚本，用于整合c++向lua提供的方法列表及将python代码统一整合到c++ dll中(以字符串形式)
"""

from time import sleep
from os import path as op

from py_integrator_builder import PyIntegratorBuilder
from native_method_collector import PyNativeMethodCollector as PyNMC

from c import Cfg


def main():
    print 'Build methods...',
    code_path = Cfg.getcodepath()
    PyNMC(op.join(code_path, 'common')).build()
    PyNMC(op.join(code_path, 'testcase')).build()
    PyNMC(op.join(code_path, 'core')).build()
    PyNMC(op.join(code_path, 'comm')).build()
    PyNMC(op.join(code_path, 'app')).build()
    print 'Done'

    print 'Build script integrator...',
    PyIntegratorBuilder.build()
    print 'Done'

    sleep(1.618)


if __name__ == '__main__':
    main()
