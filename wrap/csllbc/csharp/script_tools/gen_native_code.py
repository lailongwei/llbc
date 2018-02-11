# -*- coding: utf-8 -*-
"""
FILE    gen_native_code.py
AUTHOR  Longwei Lai<lailongwei@126.com>
DATE    2016/01/26
VERSION 1.0
BRIEF   Auto generate all C# native code.
"""

import os
from os import path as op
import shutil
import re
import md5

# This script file path
SCRIPT_PATH = op.abspath(op.dirname(__file__))

# LICENSE file path, content
LICENSE_FILE_PATH = op.join(op.dirname(op.dirname(op.dirname(op.dirname(SCRIPT_PATH)))), 'LICENSE')
LICENSE_FILE_CONTENT = open(LICENSE_FILE_PATH, 'r').readlines()
for i in range(len(LICENSE_FILE_CONTENT)):
    if i != 1:
        LICENSE_FILE_CONTENT[i] = '// ' + LICENSE_FILE_CONTENT[i]
LICENSE_FILE_CONTENT = ''.join(LICENSE_FILE_CONTENT)

# Csharp code path
CSHARP_PATH = op.dirname(SCRIPT_PATH)
# Csharp/Native directory path
CSHARP_NATIVE_PATH = op.join(CSHARP_PATH, u'native')

# C++ native code path
NATIVE_PATH = op.join(op.dirname(CSHARP_PATH), u'native')
# C++ native code header file path
NATIVE_INCLUDE_PATH = op.join(NATIVE_PATH, u'include', u'csllbc')

# All types map, use to convert C++ type to C# type.
TYPE_MAP = {
    'void': 'void',

    'char': 'byte',
    'bool': 'bool',

    'short': 'short',
    'unsigned short': 'unsigned short',
    'sint16': 'short',
    'uint16': 'unsigned short',

    'int': 'int',
    'unsigned int' : 'uint',
    'sint64': 'long',
    'uint64': 'ulong',

    'double': 'double',

    'void *': 'IntPtr',
    'char *': 'byte*',
}


# region NativeCodeInfo
class NativeFunctionInfo(object):
    """
    Native C++ function information, representation every C++ native function code.
    """

    # C++ pointer type match regular expression
    _ptr_type_re = re.compile(r'[^\*]+\s*\*')
    _deleg_type_re = re.compile(r'csllbc_Delegates::(.*)')
    _char_ptr_type_re = re.compile(r'(const)?\s*char\s*\*')

    def __init__(self):
        self.lib_name = 'NativeLibName'
        self.ret_type = 'void'
        self.method_name = 'Unknown'
        self.args = []

        # Format control data members
        self.args_multiline = False

    @property
    def cs_ret_type(self):
        """C# return type representation"""
        return self._convert_type(self.ret_type)

    @property
    def cs_args(self):
        """C# arguments type representation"""
        cs_args = []
        for arg in self.args:
            cs_args.append((self._convert_type(arg[0]), arg[1]))

        return cs_args

    def to_csharp_import_stmt(self):
        """Convert to C# DllImport statement"""
        method_attrs = '        [DllImport({}, CallingConvention = CallingConvention.Cdecl)]'.format(self.lib_name)
        method_begin = '        public extern static {} {}('.format(self.cs_ret_type, self.method_name)

        stmt = method_attrs
        stmt += '\n' + method_begin

        if self.args_multiline:
            arg_begin_pos = len(method_begin)

        cs_args = self.cs_args
        for i in xrange(len(cs_args)):
            if i != 0:
                if self.args_multiline:
                    stmt += ',\n' + ' ' * arg_begin_pos
                else:
                    stmt += ', '
            stmt += '{} {}'.format(cs_args[i][0], cs_args[i][1])

        return stmt + ');'

    @classmethod
    def _convert_type(cls, ty):
        if cls._ptr_type_re.match(ty):
            return TYPE_MAP['void *']
        else:
            deleg_m = cls._deleg_type_re.match(ty)
            if deleg_m:
                return deleg_m.group(1)
            else:
                return TYPE_MAP[ty]
 
# endregion


# region NativeCodeFile
class NativeCodeFile(object):
    """
    Native code file information, representation every C++ native code file.
    """
    _Status_Done = 0
    _Status_WaitMethodEnd = 1

    # The C# importable C++ function match regular expressions.
    _match_method = re.compile(r'\s*LLBC_EXTERN_C\s+CSLLBC_EXPORT\s+(.+)(csllbc_[^\(]+)\s*\(([^\)\\]*)(\)?)')
    _match_method_end = re.compile(r'\s*([^\)\\]*)\)')

    def __init__(self):
        self._native_header_file = ''
        self._function_infos = []
        self._csharp_code = ''
        self._parse_status = self._Status_Done

    @property
    def csharp_code_file(self):
        """Get C# code file name"""
        rel_path = self._native_header_file[len(NATIVE_INCLUDE_PATH) + 1:]
        rel_name = op.basename(rel_path)

        cs_code_file = op.splitext(rel_name)[0] + u'Native.cs'
        if cs_code_file[0:3] == u'_cs':
            cs_code_file = cs_code_file[3:]
        elif cs_code_file[0] == u'_':
            cs_code_file = cs_code_file[1:]

        return cs_code_file

    @property
    def csharp_code_path(self):
        """Get C# code file full path"""
        rel_path = self._native_header_file[len(NATIVE_INCLUDE_PATH) + 1:]
        rel_dir = op.dirname(rel_path)

        return op.join(CSHARP_NATIVE_PATH, rel_dir,  self.csharp_code_file)

    def parse(self, native_header_file):
        """Parse C++ native code file"""
        self._native_header_file = native_header_file
        with open(native_header_file, 'rb') as f:
            for line in f.readlines():
                self._parse_line(line)

        return True if self._function_infos else False

    def gen_csharp_code_file(self):
        """Generate C# code file"""
        self._csharp_code = ''

        self._begin_file()
        self._begin_class()
        self._append_methods()
        self._end_class()
        self._end_file()

        # check c# code file directory exist or not, if not exist, create it.
        code_dir = op.dirname(self.csharp_code_path)
        code_file = op.basename(self.csharp_code_path)
        if not op.exists(code_dir):
            os.makedirs(code_dir)

        # write c# code to tempory file.
        temp_code_path = op.join(code_dir, '_____' + code_file + '.temp')
        with open(temp_code_path, 'wb') as f:
            f.write(self._csharp_code)

        if not op.exists(self.csharp_code_path):
            print 'Generated native method import c# code file: {}'.format(self.csharp_code_path)
            shutil.move(temp_code_path, self.csharp_code_path)
        else:
            with open(self.csharp_code_path, 'rb') as f:
                old_digest = md5.new(f.read()).digest()
            new_digest = md5.new(self._csharp_code).digest()
            if new_digest != old_digest:
                print 'Update native method import c# code file: {}'.format(self.csharp_code_path)
                shutil.move(temp_code_path, self.csharp_code_path)
            else:
                os.remove(temp_code_path)

    # region Internal methods
    def _parse_line(self, line):
        line = line.strip()
        if self._parse_status == self._Status_Done:
            self._parse_line_new(line)
        else:
            self._parse_line_wait_methodend(line)

    def _parse_line_new(self, line):
        m = self._match_method.match(line)
        if m is None:
            return

        function_info = NativeFunctionInfo()
        function_info.ret_type = m.group(1).strip()
        function_info.method_name = m.group(2)

        args = m.group(3).strip()
        self._split_args_to_store(args, function_info)
        self._function_infos.append(function_info)

        if m.group(4) == '':
            function_info.args_multiline = True
            self._parse_status = self._Status_WaitMethodEnd
        else:
            self._parse_status = self._Status_Done

    def _parse_line_wait_methodend(self, line):
        function_info = self._function_infos[-1]
        m = self._match_method_end.match(line)
        if m is None:
            self._split_args_to_store(line, function_info)
        else:
            self._split_args_to_store(m.group(1), function_info)
            self._parse_status = self._Status_Done

    def _split_args_to_store(self, line, function_info):
        for arg in line.split(','):
            striped_arg = arg.strip()
            if not striped_arg:
                continue

            if '*' in striped_arg:
                splitted_arg = striped_arg.rsplit('*', 1)
                arg_type = splitted_arg[0].strip() + '*'
            else:
                splitted_arg = striped_arg.rsplit(' ', 1)
                arg_type = splitted_arg[0].strip()
            if len(splitted_arg) != 2:
                continue

            arg_name = splitted_arg[1].strip()
            if not arg_type or not arg_name:
                continue

            function_info.args.append((arg_type, arg_name))

    def _begin_file(self):
        a = self._append_line
        a(LICENSE_FILE_CONTENT)
        a('//!!! This file is auto generated by script tool, do not modify it!!!')
        a('')
        a('using System;')
        a('using System.Runtime.InteropServices;')
        a('')
        a('namespace llbc')
        a('{')
        a('')

    def _begin_class(self):
        a = self._append_line
        a('    /// <summary>')
        a('    /// Wrap {} file defined c functions'.format(
            self._native_header_file[len(NATIVE_INCLUDE_PATH) + 1:].replace('\\', '/')))
        a('    /// </summary>')
        a('    internal partial class LLBCNative')
        a('    {')

    def _append_methods(self):
        for i in xrange(len(self._function_infos)):
            function_info = self._function_infos[i]
            self._append_line(function_info.to_csharp_import_stmt())
            if i != len(self._function_infos) - 1:
                self._append_line('')

    def _end_class(self):
        self._append_line('    }')

    def _end_file(self):
        self._append_line('}')

    def _append_line(self, line):
        self._csharp_code += line + '\n'

    # endregion

    _Dummy = None


# endregion


def main():
    fileCount = 0
    for root, dirs, files in os.walk(NATIVE_INCLUDE_PATH):
        for name in files:
            if op.splitext(name)[1] != u'.h':
                continue

            filename = op.join(root, name)
            if u'.svn' in filename or u'.git' in filename:
                continue

            native_file = NativeCodeFile()
            if native_file.parse(filename):
                fileCount += 1
                native_file.gen_csharp_code_file()

    print 'All C# wrap code generated, file count: {}'.format(fileCount)


if __name__ == '__main__':
    main()
