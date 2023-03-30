# -*- coding: utf-8 -*-
"""
Event 测试
"""
from llbc import TestCase, Service, Event, comp, bindto

@comp
@bindto('event_test_svc')
class EventTestComp(object):
    def oninit(self, ev):
        svc = ev.svc
        print('Component initialize, svc:{}'.format(svc))

        svc.subscribe_event(10086, self._onev_10086)
        svc.subscribe_event(10010, self._onev_10010)

    def ondestroy(self, ev):
        print('Component destroy, svc:{}'.format(ev.svc))

    def onupdate(self, ev):
        svc = ev.svc
        import time
        # time.sleep(40)
        ev1 = Event(10086)
        ev1['key1'] = {'a': 3, 'b': True, 'c': {'c1': 3.5, 'c2': 'hello world'}}
        ev1['key2'] = 'the key2 value'
        svc.fire_event(ev1)

        ev2 = Event(10010)
        ev2['key1'] = 3
        ev2['key2'] = 'hello world'
        svc.fire_event(ev2)

    def _onev_10086(self, ev):
        print('recv event:{}'.format(ev))
        print('Recv 10086 event, evid:{}, key1:{}, key2:{}'.format(ev.evid, ev['key1'], ev['key2']))

    def _onev_10010(self, ev):
        print('recv event:{}'.format(ev))
        print('Recv 10010 event, evid:{}, key1{}, key2:{}'.format(ev.evid, ev['key1'], ev['key2']))


class EventTest(TestCase):
    def run(self, arg):
        print('Event test:')

        # Create service: event_test_svc
        svc = Service('event_test_svc')
        svc.start()
        svc.fps = 1

        Service.schedule()
