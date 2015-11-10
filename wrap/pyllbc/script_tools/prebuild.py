# -*- coding: utf-8 -*-
"""
预编译脚本,用于组织C++代码及python代码.
"""


from time import sleep
from os import path as op

from cfg import Cfg
from methods_builder import MethodsBuilder
from integrator_builder import IntegratorBuilder


def main():
    print 'Build methods...',
    code_path = Cfg.getcodepath()
    MethodsBuilder.build(op.join(code_path, 'common'))
    MethodsBuilder.build(op.join(code_path, 'testcase'))
    MethodsBuilder.build(op.join(code_path, 'core'))
    MethodsBuilder.build(op.join(code_path, 'comm'))
    MethodsBuilder.build(op.join(code_path, 'application'))
    print 'Done'

    print 'Build script integrator...',
    IntegratorBuilder.build()
    print 'Done'

    sleep(1.618)

if __name__ == '__main__':
    main()
