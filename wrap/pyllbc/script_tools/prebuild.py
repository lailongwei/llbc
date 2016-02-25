# -*- coding: utf-8 -*-
"""
Pre-Compile script, use to intergrate c++ code and python code.
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
