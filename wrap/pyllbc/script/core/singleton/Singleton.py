# The MIT License(MIT)

# Copyright(c) 2013 lailongwei<lailongwei@126.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files(the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions :
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# -*- coding: utf-8 -*-

import llbc

class pyllbcSingleton(object):
    _objs = {}
    
    def __new__(cls, *args, **kwargs):
        cls_dict = cls._objs.get(cls)
        if cls_dict is not None:
            return cls_dict['obj']

        obj = object.__new__(cls)
        cls._objs[cls] = {'obj': obj, 'init': False}
        setattr(cls, '__init__', cls.decorate_init(cls.__init__))
        return obj

    @classmethod
    def decorate_init(cls, func):
        def init_wrap(*args, **kwargs):
            if not cls._objs[cls]['init']:
                func(*args, **kwargs)
                cls._objs[cls]['init'] = True
            return
        return init_wrap

llbc.Singleton = pyllbcSingleton


