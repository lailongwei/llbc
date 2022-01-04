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
            print '{}(duetime:{}, period:{}): hello..., schedule times: {}'.format(
                timer.timerid, timer.duetime, timer.period, self.times)

    def oncancel(self, timer):
        pass
        # print 'timer[{}] cancelled!'.format(timer.timerid)

    def _createtimers(self):
        timers_count=1000000
        
        import time
        beg_add_time = time.time()
        print 'add {} timers:'.format(timers_count)

        random = llbc.Random()
        for i in range(0, timers_count):
            timer = llbc.Timer(self.ontimeout, self.oncancel)
            timer.schedule(random.rand(5000, 15000), random.rand(5000, 15000))

        cost_time = (time.time() - beg_add_time) * 1000
        print 'done, cost time(ms):{}'.format(cost_time)

