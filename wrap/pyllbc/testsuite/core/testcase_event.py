# -*- coding: utf-8 -*-
"""
事件相关测试代码
"""


import llbc


class EventTestCase(llbc.TestCase):
    def __init__(self):
        super(EventTestCase, self).__init__()
        self.observable = llbc.Observable()
        self.call_times = 0

    def handle(self, observable, data):
        print 'data: {}'.format(data)

        self.call_times += 1
        if self.call_times <= 10:
            if self.call_times == 3:
                self.observable.remove_observer(self.handle)
                self.observable.add_observer(self.handle)
            elif self.call_times == 5:
                self.observable.remove_all_observers()
                self.observable.add_observer(self.handle)
            self.observable.notify('a')

    def run(self, arg):
        print 'event test case:'
        self.observable.add_observer(self.handle)
        self.observable.notify('a')
