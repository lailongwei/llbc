# -*- coding: utf-8 -*-

import llbc

class pyllbcRandom(object):
    """Random class encapsulation"""
    def __init__(self, seed=0):
        """
        Create random object by random seed.
        :param seed: seed for random-number generator.
        """
        if isinstance(seed, (str, unicode)):
            seed = int(float(seed))
        self.__c_obj = llbc.inl.NewRandom(seed)

    def __del__(self):
        if llbc is not None:
            llbc.inl.DelRandom(self.__c_obj)

    def seed(self, seed):
        """
        Initialize internal state of the random number generator.
        :param seed: seed for random-number generator.
        """
        if isinstance(seed, (str, unicode)):
            seed = int(float(seed))
        llbc.inl.SeedRand(self.__c_obj, seed)

    def rand(self, begin=None, end=None):
        """
        Generate a random integer.
        if begin is None and end is None, generate a random integer N such that: -2,147,483,648 <= N < 2,147,483,647.
        if begin is not None and end is None, generate a random integer N such that: 0 <= N < end for end >= 0 and end <= N < 0 for end < 0.
        if begin is not None and end is not None, generate a random integer N such that: begin <= N < end for begin <= end and end <= N < begin for begin > end.
        :return: the random integer.
        """
        return llbc.inl.Rand(self.__c_obj, begin, end)

    def randreal(self):
        """
        Generate a floating point number N such that: 0 <= N < 1.
        :return: the random floating point number.
        """
        return llbc.inl.RandReal(self.__c_obj)

    def booljudge(self):
        """
        Execute a bool judge.
        :return: the bool judge result.
        """
        return llbc.inl.BoolJudge(self.__c_obj)

    def choice(self, seq):
        """
        Random choose one element at given sequence object(tuple, list, str, or unicode type instance).
        :return: the choiced element.
        """
        if not seq:
            return None

        return seq[self.rand(seq.__len__())]

llbc.Random = pyllbcRandom

__gbl_random = llbc.Random()

def pyllbcSeedRand(seed):
    __gbl_random.seed(seed)

llbc.seedrand = pyllbcSeedRand

def pyllbcRand(begin=None, end=None):
    return __gbl_random.rand(begin, end)

llbc.rand = pyllbcRand

def pyllbcRandReal():
    return __gbl_random.randreal()

llbc.randreal = pyllbcRandReal

def pyllbcBoolJudge():
    return __gbl_random.booljudge()

llbc.booljudge = pyllbcBoolJudge

def pyllbcChoice(seq):
    return __gbl_random.choice(seq)

llbc.choice = pyllbcChoice

