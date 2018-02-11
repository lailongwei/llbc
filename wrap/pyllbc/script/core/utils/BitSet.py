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

