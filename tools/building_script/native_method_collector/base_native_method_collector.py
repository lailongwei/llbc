# -*- coding: utf-8 -*-
"""
基础native方法收集器，完成c/c++native方法收集，并整合到对应的脚本语言中
"""

import re
from os import path as op

from cpputils import *

from c import Cfg


class BaseNativeMethodCollector(object):
    def __init__(self, search_path, classname_base=None, filename_base=None):
        self.search_path = search_path
        self.proj_name = Cfg.getprojname()
        self.search_base = op.basename(search_path).capitalize()

        if classname_base is not None:
            self.classname_base = classname_base
        else:
            self.classname_base = self.search_base

        if filename_base is not None:
            self.filename_base = filename_base
        else:
            self.filename_base = self.search_base

    def build(self):
        """构建方法文件"""
        raise NotImplementedError()

    def _buildable(self):
        if not op.exists(self.search_path):
            print 'Path [{}] not found, skip build methods...'.format(self.search_path)
            return False

        return True

    def _build_filematch_re(self):
        return re.compile(r'(_[a-zA-Z]+)+\.h')

    def _build_cpp_cls_name(self):
        return '{}_{}Methods'.format(self.proj_name, self.classname_base)

    def _build_cpp_file(self):
        cpp_file_path = op.join(self.search_path, '_{}Methods.h'.format(self.filename_base))
        cpp_file = CppFile(
            cpp_file_path, author=Cfg.getauthor(), ver=Cfg.getver(), include_macro_prefix=self.proj_name.upper())
        cpp_file.addincl('{}/common/LibHeader.h'.format(self.proj_name))
        cpp_file.addincl('{}/common/Macro.h'.format(self.proj_name))
        return cpp_file
