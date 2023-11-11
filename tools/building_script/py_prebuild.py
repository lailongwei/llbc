# -*- coding: utf-8 -*-
"""
pyllbc项目预编译脚本, 完成以下事情:
- 编译cpython submodule
- 收集pyllbc native method
- 生成python代码整合器
"""
from os import path as op

from com.cfg import cfg
from com.log import Log
from py_integrator_builder import PyIntegratorBuilder
from native_method_collector import PyNativeMethodCollector as PyNMC
from py_cpython_compiler import CPythonCompiler


def main():
    # 编译cpython submodule
    Log.i("Compile cpython submodule...")
    CPythonCompiler().compile()
    Log.i('Done!')

    # 收集pyllbc native method
    Log.i('Collect python native methods...')
    src_path = op.join(cfg.pyllbc_proj_path, 'src')
    PyNMC(op.join(src_path, 'common')).build()
    PyNMC(op.join(src_path, 'testcase')).build()
    PyNMC(op.join(src_path, 'core')).build()
    PyNMC(op.join(src_path, 'comm')).build()
    PyNMC(op.join(src_path, 'app')).build()
    Log.i('Done!')

    # 生成python代码整合器
    Log.i('Build python script integrator...')
    PyIntegratorBuilder.build()
    Log.i('Done!')


if __name__ == '__main__':
    main()
