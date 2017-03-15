# -*- coding: utf-8 -*-
"""
FILE    gen_errno_code.py
AUTHOR  Sherlock <445498529@qq.com>
DATE    2017/03/14
VERSION 1.0
BRIEF   Auto generate C# errno code.
"""

import os
from os import path as op
import shutil
import re
import md5

# This script file path
SCRIPT_PATH = op.abspath(op.dirname(__file__))

# LLBC root path
LLBC_ROOT_PATH = op.normpath(op.join(SCRIPT_PATH, u'../../../../'))

# CPP errno file name
CPP_ERRNO_FILE = op.join(LLBC_ROOT_PATH, u'llbc', u'include', u'llbc', u'common', u'Errno.h')

# Csharp code path
CS_ERRNO_FILE = op.join(LLBC_ROOT_PATH, u'wrap', u'csllbc', u'csharp', u'native', u'common', u'ErrnoNative.cs')

# analyse csharp file
def analyse_file(name):
    f = open(name)
    if not f:
      print 'file not found: ' + name
      exit()
      
    ls = []
    spc = False
    
    for line in f:
      result = re.match(r'#define LLBC_ERROR_([^\s]*)[\s]*(.*)', line)
      if not result:
        if spc == False:
          ls.append('\n')
          spc = True
        continue

      if re.match(r'[^\(]*\([^\)]*\)', result.group(1)):
        continue
      
      key = result.group(1)
      value = result.group(2)
      result = re.match(r'\(\([^\)]*\)\(([^\)]*)\)\)', value)
      if result:
        value = result.group(1)
      
      str = '        static public uint LLBC_ERROR_' + key
      size = len(key)
      if size < 24:
        for i in range(1, 24 - size):
          str = str + ' '
          
      ls.append(str + '= ' + value + ';\n')
      spc = False
          
    f.close()
    return ls

# get file md5 set
def get_file_md5_set(name):
    if not os.path.exists(name):
      return 0

    f = open(name, 'rb')
    md5set = md5.new(f.read()).digest()
    f.close()

    return md5set

# generate c++ head file    
def generate_file(name, ls):
    oldset = get_file_md5_set(name)

    text = 'using System;\n'
    text = text + '\n'
    text = text + 'namespace llbc\n'
    text = text + '{\n'
    text = text + '    public class Errno\n'
    text = text + '    {\n'
    for line in ls:
      text = text + line
    text = text + '    }\n'
    text = text + '}\n'
    
    if md5.new(text).digest() == oldset:
      return 'Errno: file check finished!'
    
    f = open(name, 'wb')
    if not f:
      return 'Errno: file open failure: ' + name
    
    f.write(text)
    f.close()
    return 'Errno: grenerate finished!\n  at: ' + name

def main():
    
    ls = analyse_file(CPP_ERRNO_FILE)
      
    str = generate_file(CS_ERRNO_FILE, ls)
    
    if str != '':
      print str

if __name__ == '__main__':
    main()
