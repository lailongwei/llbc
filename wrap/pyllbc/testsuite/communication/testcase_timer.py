# -*- coding: utf-8 -*-
"""
Timer测试代码
"""

import llbc


class TimerTest(llbc.TestCase):
    def __init__(self):
        super(TimerTest, self).__init__()
        self.times = 0

    def run(self, arg):
        print 'timer test:'
        self._createtimers()
        llbc.Service.schedule()

    def ontimeout(self, timer):
        self.times += 1
        if self.times % 10000 == 0:
            print '{}(period:{}): hello..., schedule times: {}'.format(timer.timerid, timer.period, self.times)

    def oncancel(self, timer):
        pass
        # print 'timer[{}] cancelled!'.format(timer.timerid)

    def _createtimers(self):
        for i in range(0, 500000):
            timer = llbc.Timer(self.ontimeout, self.oncancel)
            timer.schedule(llbc.Random.rand(5000, 15000), llbc.Random.rand(5000, 15000))

