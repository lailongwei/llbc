# -*- coding: utf-8 -*-
"""
 lullbc预编译脚本
"""

from os import path as op

from com.log import Log
from com.cfg import cfg
from com.defs import ProjType
from lu_script_integrator import LuScriptIntegrator
from native_method_collector import LuNativeMethodCollector as LuNMC


def main():
    # 收集所有lua native methods并进行整合
    Log.i('Collect lua native methods...')
    src_path = op.join(cfg.lullbc_proj_path, 'src')
    LuNMC(src_path,
          classname_base='',
          filename_base=ProjType.lullbc).build()
    Log.i('Done!')

    # 整合所有lua脚本
    Log.i('Integrate lua script...')
    LuScriptIntegrator().build()

    Log.i('Done!')


if __name__ == '__main__':
    main()
