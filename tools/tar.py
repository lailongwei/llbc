#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
llbc project tarball util, use for tarball llbc project.
"""

import os
from os import path as op
from time import time
from datetime import datetime
import tarfile
from tarfile import TarInfo
import sys
import platform

CUR_DIR = op.dirname(op.abspath(__file__))
PROJ_DIR = op.dirname(CUR_DIR)
NOW = datetime.fromtimestamp(time())
TARBALL = op.join(op.dirname(PROJ_DIR), 'llbc_{0}.tar.bz2'.format(NOW.strftime('%Y%m%d_%H%M%S')))
TAR_GIT = True if (len(sys.argv) == 1 or (len(sys.argv) > 1 and sys.argv[1].lower() == 'true')) else False
FILTEROUT_SUFFIXES = ('.obj', '.pyc', '.sdf', '.opensdf', '.suo', '.user', '.ncb', '.cache', '.o', '.d')
OS_NAME = platform.uname()[0].lower()

def common_filter(tarinfo):
    """
    Common tarinfo filter, use to all files/directories.
    """
    assert isinstance(tarinfo, TarInfo)
    if tarinfo.name.find('.svn') != -1:
        return None
    elif tarinfo.name.find('.git') != -1:
        return None

    if OS_NAME != 'windows' and \
            tarinfo.type == tarfile.DIRTYPE:
        # filter-out llbc/<platform_name>, testsuite/<platform_name>, wrap/xxx/<platform_name>
        if tarinfo.name == 'llbc/{0}'.format(OS_NAME) or \
                tarinfo.name.startswith('testsuite/{0}'.format(OS_NAME)) or \
                tarinfo.name.startswith('wrap/pyllbc/{0}'.format(OS_NAME)) or \
                tarinfo.name.startswith('wrap/csllbc/native/{0}'.format(OS_NAME)):
            return None

        # filter-out llbc/lib, testsuite/lib, wrap/xxx/lib
        if tarinfo.name.startswith('llbc/lib') or \
                tarinfo.name.startswith('testsuite/bin') or \
                tarinfo.name.startswith('wrap/pyllbc/lib') or \
                tarinfo.name.startswith('wrap/csllbc/native/lib'):
            return None

    basename = op.basename(tarinfo.name)
    suffix = op.splitext(basename)[1].lower()
    if suffix in FILTEROUT_SUFFIXES:
        return None

    return tarinfo


def vsprojs_filter(tarinfo):
    """
    vsprojs tarinfo filter, only use for vsxxxx directories.
    """
    assert isinstance(tarinfo, TarInfo)
    if tarinfo.type == tarfile.DIRTYPE:
        dirname = op.basename(tarinfo.name)
        if len(dirname) <= 2 or dirname[0:2] != 'vs':
            return None

    return common_filter(tarinfo)


def main():
    old_cwd = os.getcwd()
    tar = tarfile.open(TARBALL, 'w:bz2')

    os.chdir(PROJ_DIR)

    try:
        # tar llbc/wrap/tools/testsuite
        print 'tar llbc...'
        tar.add('llbc', filter=common_filter)
        print 'tar wrap...'
        tar.add('wrap', filter=common_filter)
        print 'tar tools...'
        tar.add('tools', filter=common_filter)
        print 'tar testsuite...'
        tar.add('testsuite', filter=common_filter)

        # tar llbc.xcodeproj
        # TODO: Will delete
        tar.add('llbc.xcodeproj')

        # tar .git directory, if exists and specified
        if TAR_GIT and op.exists('.git'):
            print 'tar .git...'
            tar.add('.git')
            if op.exists('.gitignore'):
                tar.add('.gitignore')

        # tar vsprojs
        for dir in os.listdir('vsprojs'):
            tardir = 'vsprojs/{0}'.format(dir)
            if not op.isdir(tardir):
                continue

            print 'tar {0}...'.format(tardir)
            tar.add(tardir, filter=vsprojs_filter)

        # tar others
        print 'tar other files...'
        tar.add('CHANGELOG')
        tar.add('LICENSE')
        tar.add('Makefile')
        tar.add('README.md')
        tar.add('rules.mk')

        tar.close()
        print 'Done, tar file: {0}'.format(TARBALL)
    finally:
        os.chdir(old_cwd)

    return


if __name__ == '__main__':
    main()
