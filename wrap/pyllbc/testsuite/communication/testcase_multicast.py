# -*- coding: utf-8 -*-

from sys import argv as _argv
from llbc import TestCase, Service, handler, forsend, forrecv, bindto


_OPCODE = 1  # 测试操作码
_CONN_COUNT = 50  # 连接数


@forsend(_OPCODE)
@forrecv(_OPCODE)
@bindto('multicast_test_svc')
class _TestMsg(object):
    def __init__(self):
        self._val = 'hello world!'

    def encode(self, s):
        s.pack('S#', self._val)

    def decode(self, s):
        self._val, = s.unpack('S#')

    def __str__(self):
        return self._val

    def __repr__(self):
        return self._val


class _TestComp(object):
    def __init__(self, asClient, useBst):
        self._asClient = asClient
        self._useBst = useBst

        self._session_ids = list()

    def onsessioncreate(self, ev):
        if self._asClient:
            return

        if not ev.islisten:
            self._session_ids.append(ev.session_id)

    def onsessiondestroy(self, ev):
        if self._asClient:
            return

        self._session_ids.remove(ev.session_id)

    def onupdate(self, ev):
        if self._asClient:
            return

        svc = ev.svc
        if self._useBst:
            svc.broadcast(_TestMsg(), _OPCODE)
        else:
            svc.multicast(self._session_ids, _TestMsg(), _OPCODE)


@handler(_OPCODE)
@bindto('multicast_test_svc')
class MsgHandler(object):
    def __call__(self, packet):
        print 'client[{:4d} received packet: {}]'.format(packet.session_id, packet.data)


class MulticastTest(TestCase):
    def __init__(self):
        super(MulticastTest, self).__init__()
        self._as_client = False
        self._ip = ''
        self._port = 0
        self._use_bst = False

        self._svc = None

    def run(self, *args, **kwargs):
        if len(_argv) < 4:
            print 'argument error, eg: python main.py server/client ip port [useBst = False]'
            return -1

        self._fetch_args()

        self._svc = Service('multicast_test_svc')

        svc = self._svc
        svc.addcomp(_TestComp(self._as_client, self._use_bst))

        if self._as_client:
            self._start_as_client()
        else:
            self._start_as_server()

        Service.schedule()

    def _fetch_args(self):
        self._as_client = True if _argv[1].lower() == 'client' else False
        self._ip = str(_argv[2])
        self._port = int(_argv[3])
        if len(_argv) >= 5:
            self._use_bst = True if _argv[4].lower() == 'true' else False

    def _start_as_client(self):
        svc = self._svc
        for i in xrange(_CONN_COUNT):
            svc.connect(self._ip, self._port)

        svc.start()

        print 'client startup, connected to {}:{}'.format(self._ip, self._port)

    def _start_as_server(self):
        svc = self._svc
        svc.listen(self._ip, self._port)
        svc.start()

        print 'server startup, listened in {}:{}'.format(self._ip, self._port)
