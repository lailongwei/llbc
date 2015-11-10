# -*- coding: utf-8 -*-
"""
自定义包头测试
"""
from llbc import TestCase, Service, forsend, bindto, forrecv, handler, PacketHeaderDesc, PacketHeaderPartDesc, \
    Random
import sys
from time import time

_OPCODE = 0


class _CustomPart(object):
    def __str__(self):
        return 'llbc'


@forsend(_OPCODE)
@forrecv(_OPCODE)
@bindto('customheaderparts_test_svc')
class _Msg(object):
    def __init__(self):
        self.val = 'Hello World'

    def encode(self, s):
        s.pack('S#', self.val)

    def decode(self, s):
        self.val = s.unpackone('S#')

    def __str__(self):
        return self.val


@handler(_OPCODE)
@bindto('customheaderparts_test_svc')
class _Handler(object):
    def __call__(self, packet):
        svc = packet.svc
        if svc.type == Service.RAW:
            data = _Msg()
            data.decode(packet.data)
        else:
            data = packet.data

        print '[{:3d}]: Receive data: {}'.format(packet.session_id, data)

        if svc.type != Service.RAW:
            print '  part 2 value(int): {}'.format(packet.get_headerpart_as_int(2))
            print '  part 3 value(int): {}'.format(packet.get_headerpart_as_int(3))
            print '  part 4 value(float): {}'.format(packet.get_headerpart_as_float(4))
            print '  part 5 value(float): {}'.format(packet.get_headerpart_as_float(5))
            print '  part 6 value(str): {}'.format(packet.get_headerpart_as_str(6))
            print '  part 7 value(str): {}'.format(packet.get_headerpart_as_str(7))
            print '  part 8 value(str): {}'.format(packet.get_headerpart_as_str(8))


class _Facade(object):
    def __init__(self, as_client, test_type):
        self._as_client = as_client
        self._test_type = test_type

        if not self._as_client:
            self._connected_session_ids = list()
            self._last_send_time = 0

    def oninitialize(self, ev):
        print 'Service init'

    def ondestroy(self, ev):
        print 'Service destroy'

    def onupdate(self, ev):
        if self._as_client:
            return
        elif not self._connected_session_ids:
            return

        now = time()
        if now - self._last_send_time < 0.5:
            return

        msg = _Msg()
        msg.val = "The message, construct time: {}".format(now)

        parts = self._build_parts()

        svc = ev.svc
        assert isinstance(svc, Service)
        if self._test_type == 'send':
            session_id = Random.choice(self._connected_session_ids)
            svc.send(session_id, msg, opcode=_OPCODE, parts=parts)
        elif self._test_type == 'multicast':
            svc.multicast(self._connected_session_ids, msg, opcode=_OPCODE, parts=parts)
        else:
            svc.broadcast(msg, opcode=_OPCODE, parts=parts)

        self._last_send_time = now

    def onidle(self, ev):
        pass
        # print 'Service idle'

    def onsessioncreate(self, ev):
        if self._as_client:
            return
        elif ev.islisten:
            return

        session_id = ev.session_id
        print 'Session created, id: {}'.format(session_id)
        if ev.islisten:
            return

        self._connected_session_ids.append(session_id)

    def onsessiondestroy(self, ev):
        if self._as_client:
            return
        elif ev.islisten:
            return

        session_id = ev.session_id
        print 'Session destroy, id: {}'.format(session_id)
        if ev.islisten:
            return

        self._connected_session_ids.remove(ev.session_id)

    @staticmethod
    def _build_parts():
        parts = {}
        # Set part 2 value, part 2 size: 4 bytes.
        parts[2] = 32
        # Set part 3 value, part 3 size: 8 bytes.
        parts[3] = 3000000000L
        # Set part 4 value, part 4 size: 4 bytes
        parts[4] = 4.816
        # Set part 5 value, part 5 size: 8 bytes
        parts[5] = 8.1632
        # Set part 6 value, part 6 size: 4 bytes
        parts[6] = _CustomPart()
        # Set part 7 value, part 7 size: 13
        parts[7] = 'hello python!'
        # Set part 8 value, part 7 size: 20
        parts[8] = bytearray('hello bytearray!')

        return parts


class CustomHeaderPartsTest(TestCase):
    def __init__(self):
        super(CustomHeaderPartsTest, self).__init__()
        self._as_client = False
        self._is_raw = False
        self._test_type = 'send'  # send/multicast/broadcast
        self._ip = '127.0.0.1'
        self._port = 7788

    def run(self, arg):
        print 'custom header parts test:'
        if not self._parse_args():
            return

        self._design_header()
        self._startup_svc()

        Service.schedule()

        print "press any key to exit..."
        raw_input()

    def _parse_args(self):
        if len(sys.argv) < 6:
            print 'argument error, eg: ./a client/server raw/normal ip port send/multicast/broadcast'
            return False

        self._as_client = True if sys.argv[1].lower() == 'client' else False
        self._is_raw = True if sys.argv[2].lower() == 'raw' else False
        self._test_type = sys.argv[5].lower().strip()

        self._ip = sys.argv[3]
        self._port = int(sys.argv[4])

        return True

    def _design_header(self):
        header_desc = PacketHeaderDesc()

        # part 0: length part
        len_part = PacketHeaderPartDesc()
        len_part.serial_no = 0
        len_part.part_len = 4
        len_part.is_len_part = True
        header_desc.add_part(len_part)

        # part 1: opcode part
        opcode_part = PacketHeaderPartDesc()
        opcode_part.serial_no = 1
        opcode_part.part_len = 2
        opcode_part.is_opcode_part = True
        opcode_part.is_len_included_self = False
        header_desc.add_part(opcode_part)

        # part 2: 4 bytes part, use to store sint32 value.
        sint32_part = PacketHeaderPartDesc()
        sint32_part.serial_no = 2
        sint32_part.part_len = 4
        sint32_part.is_len_included_self = False
        header_desc.add_part(sint32_part)

        # part 3: 8 bytes part, use to store sint64 value.
        sint64_part = PacketHeaderPartDesc()
        sint64_part.serial_no = 3
        sint64_part.part_len = 8
        header_desc.add_part(sint64_part)

        # part 4: 4 bytes part, use to store float4 value.
        float4_part = PacketHeaderPartDesc()
        float4_part.serial_no = 4
        float4_part.part_len = 4
        header_desc.add_part(float4_part)

        # part 5: 8 bytes part, use to store float8 value.
        float8_part = PacketHeaderPartDesc()
        float8_part.serial_no = 5
        float8_part.part_len = 8
        header_desc.add_part(float8_part)

        # part 6: 4 bytes part, use to store string: 'llbc'.
        llbcstr_part = PacketHeaderPartDesc()
        llbcstr_part.serial_no = 6
        llbcstr_part.part_len = 4
        llbcstr_part.is_len_included_self = False
        header_desc.add_part(llbcstr_part)

        # part 7: 13 bytes part, use to store string: 'hello python!'
        stdstr_part = PacketHeaderPartDesc()
        stdstr_part.serial_no = 7
        stdstr_part.part_len = 13
        header_desc.add_part(stdstr_part)

        # part 8: 20 bytes part, use to store string: 'hello bytearray!'
        bytearray_part = PacketHeaderPartDesc()
        bytearray_part.serial_no = 8
        bytearray_part.part_len = 20
        header_desc.add_part(bytearray_part)

        Service.set_header_desc(header_desc)

    def _startup_svc(self):
        # Create service
        svc = Service('customheaderparts_test_svc',
                      Service.RAW if self._is_raw else Service.NORMAL)

        # Create facade
        facade = _Facade(self._as_client, self._test_type)
        svc.registerfacade(facade)

        svc_type_repr = 'RAW' if self._is_raw else 'NORMAL'
        if self._as_client:
            while True:
                try:
                    session_id = svc.connect(self._ip, self._port)
                except (Exception, ), e:
                    print 'Connect to {}:{} failed, will continue to try...'.format(self._ip, self._port)
                else:
                    print 'Service[{}] connected to {}:{}, session_id: {}' \
                        .format(svc_type_repr, self._ip, self._port, session_id)
                    break
        else:
            svc.listen(self._ip, self._port)
            print 'Service[{}] listened on {}:{}' \
                .format(svc_type_repr, self._ip, self._port)

        svc.start()

