# -*- coding: utf-8 -*-
"""
shell简易包装
"""
import os

from com.log import Log


class Sh(object):
    @staticmethod
    def execute(cmd):
        Log.fd('Exec sh cmd: {}'.format(cmd))
        return os.system(cmd)
