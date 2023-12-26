# -*- coding: utf-8 -*-
"""
Service basic 测试
"""

import traceback
import sys
from llbc import TestCase, comp, packet, bindto, Service, Stream, handler, exc_handler

@comp
@bindto('svcbase_test_svc')
class TestComp(object):
    def __init__(self):
        self._call_times = 0
        self._init_times = 0
        self._start_times = 0
        self._stop_times = 0
        self._destroy_times = 0

    def oninit(self, ev):
        print 'service init: {}'.format(ev)
        if self._init_times == 10:
            return True
        else:
            self._init_times += 1
            return False

    def onstart(self, ev):
        print 'service start: {}'.format(ev)
        if self._start_times == 10:
            return
        else:
            self._start_times += 1
            return False

    def onstop(self, ev):
        print 'service stop: {}'.format(ev)
        if self._stop_times == 10:
            return True
        else:
            self._stop_times += 1
            return False

    def ondestroy(self, ev):
        print 'service destroy: {}'.format(ev)
        if self._destroy_times == 10:
            pass
        else:
            self._destroy_times += 1
            return False

    def onupdate(self, ev):
        # print 'service update: {}'.format(ev)
        self._call_times += 1
        if self._call_times == 300:
            print 'Service update {} times, deschedule it'.format(self._call_times)
            Service.deschedule()

    def onidle(self, ev):
        pass
        # print 'service idle: {}'.format(d)

    def onsessioncreate(self, ev):
        print 'new session create: {}'.format(ev)

    def onsessiondestroy(self, ev):
        print 'session destroy, {}'.format(ev)

    def onasyncconnresult(self, ev):
        print 'async-conn result: {}'.format(ev)

    def onunhandledpacket(self, ev):
        print 'unhandled packet: {}'.format(ev)

    def onprotoreport(self, ev):
        print 'protocol report: {}'.format(ev)


@packet
@bindto('svcbase_test_svc')
class TestData(object):
    def __init__(self):
        self.iVal = 3
        self.strVal = 'hello pyllbc'
        self.listVal = [1, 3, 4, 5]
        self.dictVal = {1: 'hello', 2: 'python'}

    def encode(self, s):
        assert isinstance(s, Stream)
        s.pack('iS[i]{i:S}', self.iVal, self.strVal, self.listVal, self.dictVal)

    def decode(self, s):
        assert isinstance(s, Stream)
        self.iVal, self.strVal, self.listVal, self.dictVal = s.unpack('iS[i]{i:S}')

    def __str__(self):
        return 'iVal:{},strVal:{}, listVal:{},dictVal:{}'.format(self.iVal, self.strVal, self.listVal, self.dictVal)


@handler(TestData)
@bindto('svcbase_test_svc')
class TestHandler(object):
    def __init__(self):
        self._handleTimes = 0

    def __call__(self, packet):
        svc = packet.svc
        data = packet.data
        session_id = packet.session_id
        # print 'session[{}] recv data, opcode:{}, pkt:{}'.format(session_id, packet.opcode, packet)

        # print 'send response...'
        data.iVal = self._handleTimes
        svc.send(session_id, data)
        # raise Exception('Test exception, raise from TestData packet handler')

        self._handleTimes += 1
        if self._handleTimes % 5000 == 0:
            print('Handled {} times, test data:{}'.format(self._handleTimes, data))

@exc_handler(TestData)
@bindto('svcbase_test_svc')
class ExcHandler(object):
    def __call__(self, svc, tb, e):
        print 'Exc handler, e: {}, tb:'.format(e)
        traceback.print_tb(tb)

class SvcBaseTest(TestCase):
    def misc_test(self):
        s = '11111111'
        f = open('aaa')
        f.seek(0)
        s.count('1')

    def run(self, arg):
        print 'Service base test:'

        # Create service: my_svc
        svc = Service('svcbase_test_svc')
        svc.start()

        # Listen
        ip = '127.0.0.1'
        port = 7788
        listen_sid = svc.listen(ip, port)
        print 'Service {} listening in {}:{}, session Id: {}'.format(svc.name, ip, port, listen_sid)

        # Connect to listen session.
        conn_sid = svc.connect(ip, port)
        print 'Connect to {}:{} success, sid: {}'.format(ip, port, conn_sid)
        
        # Set service fps.
        svc.fps = 200

        # Send data.
        for i in range(200):  # Note: You can modify range limit to execute performance test.
            svc.send(conn_sid, TestData())
        # Test unhandled packet.
        svc.send(conn_sid, data=3, opcode=10086, status=0)

        # Create service: another_svc
        another = Service('another')
        another.start()

        # Schedule.
        Service.schedule()

        print('deschedule!!!!!!!!!!!!')
        print(sys.getrefcount(svc))
        print(sys.getrefcount(another))
        # If deschedule, will gone to here.
        # Delete my_svc service.
        del Service.svcbase_test_svc
        del Service.another
        print('deschedule finish!!!!!!!!!!!!')
        print(sys.getrefcount(svc))
        print(sys.getrefcount(another))
