# -*- coding: utf-8 -*-

from llbc import Service, TestCase
from time import sleep
import llbc

TEST_REQ = 1
TEST_RES = 51

TEST_IP = '127.0.0.1'
TEST_PORT = 17788


class ClientComp(object):
    def oninit(self, ev):
        ev.svc.asyncconn(TEST_IP, TEST_PORT)

    def onsessioncreate(self, ev):
        data = 'hello'
        print 'Client session create, id: {}, send data: {}'.format(ev.session_id, data)
        ev.svc.send(session_id=ev.session_id, data=data, opcode=TEST_REQ)

    def onsessiondestroy(self, ev):
        print 'Client session destroy, id: {}'.format(ev.session_id)

    def onasyncconnresult(self, ev):
        print 'Client async-conn result, connected: {}, reason: {}'.format(ev.connected, ev.reason)


class ClientPacketHandler(object):
    def __call__(self, packet):
        payload = packet.data
        data = payload.unpackstr()
        payload.pos = 0
        print 'Client recv packet: {}'.format(data)
        packet.svc.send(session_id=packet.session_id, data=data, opcode=TEST_REQ)


class ClientPacketPreHandler(object):
    def __call__(self, packet):
        payload = packet.data
        data = packet.unpackstr()
        payload.pos = 0
        print 'Client prev-recv packet: {}'.format(data)
        return True


class ClientPacketUnifyPreHandler(object):
    def __call__(self, packet):
        payload = packet.data
        data = payload.unpackstr()
        payload.pos = 0
        print 'Client unify prev-recv packet: {}(len:{}), stream.pos:{}, stream.cap:{}'.format(
            data, data.__len__(), payload.pos, payload.cap)
        return True


class ServerComp(object):
    def onsessioncreate(self, ev):
        print 'Server session create, id: {}'.format(ev.session_id)

    def onsessiondestroy(self, ev):
        print 'Server session destroy, id: {}'.format(ev.session_id)


class ServerPacketHandler(object):
    def __call__(self, packet):
        data = packet.data.unpackstr()
        print 'Server recv packet: {}'.format(data)

        sleep(1)
        packet.svc.send(session_id=packet.session_id, data=data, opcode=TEST_RES)


class ServerPacketPreHandler(object):
    _RECV_TIMES_LIMIT = 10

    def __init__(self):
        self._times = 0

    def __call__(self, packet):
        print 'Server pre-recv packet: {}'.format(packet.data)
        self._times += 1
        if self._times == self._RECV_TIMES_LIMIT:
            print 'Server recv times >= {}, will remove this session'.format(self._RECV_TIMES_LIMIT)
            return False

        return True


class ServerPacketUnifyPreHandler(object):
    _RECV_TIMES_LIMIT = 100

    def __init__(self):
        self._times = 0

    def __call__(self, packet):
        print 'Server unify pre-recv packet: {}'.format(packet.data)
        self._times += 1
        if self._times == self._RECV_TIMES_LIMIT:
            print 'Server recv times >= {}, will remove this session'.format(self._RECV_TIMES_LIMIT)
            return False

        return True


class MultiSvcTest(TestCase):
    def run(self, arg):
        print 'multi service test:'

        client = Service('multisvc_test_client')
        server = Service('multisvc_test_server')

        client.addcomp(ClientComp())
        server.addcomp(ServerComp())

        client.subscribe(TEST_RES, ClientPacketHandler())
        server.subscribe(TEST_REQ, ServerPacketHandler())

#        client.presubscribe(TEST_RES, ClientPacketPreHandler())
#        server.presubscribe(TEST_REQ, ServerPacketPreHandler())

        try:
            client.unify_presubscribe(ClientPacketUnifyPreHandler())
            server.unify_presubscribe(ServerPacketUnifyPreHandler())
        except llbc.error, e:
            print 'subscribe unify pre-handler to service failed'
            print 'reason: {}'.format(e)

        server.start()
        server.listen(TEST_IP, TEST_PORT)

        client.start()

        Service.schedule()
