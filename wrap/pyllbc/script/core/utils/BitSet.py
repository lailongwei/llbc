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

class pyllbcBitSet(object):
    """
    pyllbc bitset class encapsulation.
    """
    def __init__(self, init_bits=0):
        self._bits = long(init_bits)

    @property
    def bits(self):
        return self._bits

    def set_bits(self, bits):
        self._bits |= bits

    def unset_bits(self, bits):
        self._bits &= (~bits)

    def swap_bits(self, from_bits, to_bits):
        self.unset_bits(from_bits)
        self.set_bits(to_bits)

    def has_bits(self, bits):
        return True if self._bits & bits == bits else False

    def only_has_bits(self, bits):
        if self._bits & bits != bits:
            return False
        elif self._bits & (~bits) != 0:
            return False
        return True

    def set_all(self):
        self._bits = -1

    def unset_all(self, reset_bits=0):
        self._bits = reset_bits

    def __str__(self):
        return self._i2b(self._bits)

    @staticmethod
    def _i2b(bits):
        l = []
        n, m = divmod(bits, 2)
        l.append(str(m))
        while n != 0:
            n, m = divmod(n, 2)
            l.insert(0, str(m))

        return ''.join(l)

llbc.BitSet = pyllbcBitSet

