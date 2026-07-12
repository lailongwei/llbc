# -*- coding: utf-8 -*-
"""
unit_test项目预编译脚本, 用于:
- 完成 gtest/gmock 测试框架编译
"""

import os
from os import path as op
import re

from com.sh import Sh
from com.log import Log
from com.cfg import cfg
from com.defs import PlatformType


def _compile_gtest():
    """编译 gtest/gmock 测试框架"""
    if cfg.platform == PlatformType.Windows:
        _compile_gtest_win32()
    else:
        _compile_gtest_non_win32()


def _compile_gtest_win32():
    """
    编译 gtest/gmock 测试框架 - Win32 平台
    """
    raise Log.e('Win32 platform gtest/gmock framework compile is not implemented for now')

def _compile_gtest_non_win32():
    """
    编译 gtest/gmock 测试框架 - Non-Win32 平台
    """

    # Log 信息
    gtest_fwk_path = op.join(cfg.llbc_test_projs_3rdparty_dir, 'googletest')
    custom_ccpp_toolset_bin_path = cfg.custom_ccpp_toolset_bin_path
    Log.t('- Compiling googletest framework')
    Log.t('    >> framework path: {}'.format(gtest_fwk_path))
    Log.t('    >> disable cxx11 abi: {}'.format(cfg.disable_cxx11_abi))
    Log.t('    >> custom ccpp toolset bin path: {}'.format(custom_ccpp_toolset_bin_path))

    # 辅助函数: 获取 cmake list lines
    gtest_cmake_lists_file_path = op.join(gtest_fwk_path, 'CMakeLists.txt')
    def _read_cmake_lists_file():
        lines = []
        with open(gtest_cmake_lists_file_path, 'r') as f:
            for line in f.readlines():
                lines.append(line.rstrip('\r\n'))
        return lines 

    # 辅助函数: 回写 cmake list file
    def _update_cmake_lists_file(lines):
        with open(gtest_cmake_lists_file_path, 'w') as f:
            for line in lines:
                f.write(line + '\n')

    # 辅助函数: 于 project() 前插入内容
    def _insert_lines(file_lines, insert_lines):
        project_line_idx = -1
        c = re.compile('^\s*project\(\s*([^)]+)\s*\)\s*(#.*)?$')
        for idx in range(0, len(file_lines)):
            line = file_lines[idx]
            if c.match(line) is not None:
                project_line_idx = idx
                break

        if project_line_idx == -1:
            Log.e('    - Not find project(xxx) line')
            return

        file_lines[project_line_idx:project_line_idx] = insert_lines

    # region 设置自定义编译工具集
    # 如有 custom ccpp toolset bin path, 更新 googletest/CMakeLists.txt.
    if op.exists(custom_ccpp_toolset_bin_path):

        # 得到 CMakeLists.txt 文件内容
        cmake_lists_file_lines = _read_cmake_lists_file()

        # 查找是否已经有旧的, 如有且已经较旧(跟 premake5_cfg.lua 内设置不同), 则更新
        updated = False
        need_write_back = False
        c = re.compile('^\s*set\(LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH\s+"?([^"#]+?)"?\s*\)\s*(#.*)?$')
        for idx in range(0, len(cmake_lists_file_lines)):
            line = cmake_lists_file_lines[idx]
            m = c.match(line)
            if not m:
                continue

            if (m.group(1) != custom_ccpp_toolset_bin_path):
                Log.i('  - Update LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH: {} -> {}'.format(m.group(1), custom_ccpp_toolset_bin_path))
                cmake_lists_file_lines[idx] = 'set(LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH "{}"){}{}'.format(
                    custom_ccpp_toolset_bin_path, m.group(2) and ' ' or '', m.group(2) or '')
                need_write_back = True
            else:
                Log.t('  - LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH is already set to: {}'.format(custom_ccpp_toolset_bin_path))

            updated = True
            break

        # 无， 则在 project(xxx) 行前插入
        if not updated:
            Log.t('  - Not find LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH, update CMakeLists.txt')
            insert_lines = [
                '# Custom ccpp toolset bin path(add by llbc unit_test prebuild script)',
                'set(LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH "{}")'.format(custom_ccpp_toolset_bin_path),
                'set(CMAKE_CXX_COMPILER "${LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH}/g++")',
                'set(CMAKE_C_COMPILER "${LLBC_CUSTOM_CCPP_TOOLSET_BIN_PATH}/gcc")',
                ''
            ]
            _insert_lines(cmake_lists_file_lines, insert_lines)
            
            need_write_back = True

        # 如需要回写 CMakelists.txt, 回写
        if need_write_back:
            _update_cmake_lists_file(cmake_lists_file_lines)
    # endregion

    # region 启用/禁用 cxx11 abi
    def _update_cxx11_abi_setting():
        disable_cxx11_abi_setting_line_idx = -1
        cmake_lists_file_lines = _read_cmake_lists_file()
        for idx in range(0, len(cmake_lists_file_lines)):
            line = cmake_lists_file_lines[idx]
            if '-D_GLIBCXX_USE_CXX11_ABI=0' in line:
                disable_cxx11_abi_setting_line_idx = idx
                break

        need_write_back = False
        if cfg.disable_cxx11_abi and disable_cxx11_abi_setting_line_idx == -1:
            Log.t('  - Disable cxx11 abi')
            insert_lines = [
                'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")'
            ]
            _insert_lines(cmake_lists_file_lines, insert_lines)

            need_write_back = True
        elif not cfg.disable_cxx11_abi and disable_cxx11_abi_setting_line_idx != -1:
            Log.t('  - Enable cxx11 abi')
            del cmake_lists_file_lines[disable_cxx11_abi_setting_line_idx]

            need_write_back = True

        if need_write_back:
            _update_cmake_lists_file(cmake_lists_file_lines)
    _update_cxx11_abi_setting()

    # endregion

    # 编译 googletest.
    cmake_building_dir = op.join(gtest_fwk_path, 'build')
    if not op.exists(cmake_building_dir):
        Log.t('  - Create googletest cmake building dir: {}'.format(cmake_building_dir))
        os.makedirs(cmake_building_dir)

    # 进入 cmake building dir, 执行编译
    compile_cmd = 'pushd {} && cmake .. && make && popd > /dev/null'.format(cmake_building_dir)
    ret = Sh.execute(compile_cmd)
    if ret != 0:
        Log.e('  - Compile googletest framework failed, ret code:{}'.format(ret))
        return

    Log.t('  - Compile googletest framework success')


def main():
    # Log 开始 prebild.
    Log.i('Begin prebuild unit_test project')

    # 编译 gtest.
    _compile_gtest()

    Log.i('unit_test prebuild success!')

if __name__ == '__main__':
    main()