# -*- coding: utf-8 -*-

import llbc

class pyllbcPacket(object):
    def __init__(self, svc, session_id, local_ip, local_port, peer_ip, peer_port, opcode, status, status_desc, data, packet_cobj):
        self.svc = svc
        self.session_id = session_id
        self.local_ip = local_ip
        self.local_port = local_port
        self.peer_ip = peer_ip
        self.peer_port = peer_port
        self.opcode = opcode
        self.status = status
        self.status_desc = status_desc
        self.data = data
        self._packet_cobj = packet_cobj

        self._str = 'session_id: {}, local: {}:{}, peer: {}:{}, opcode: {}, status: {}, status_desc: {}, data: {}'.format(
                session_id, local_ip, local_port, peer_ip, peer_port, opcode, status, status_desc, data)

    def get_headerpart_as_int(self, serial_no):
        return llbc.inl.Packet_GetHeaderPartAsSInt64(self._packet_cobj, serial_no)

    def get_headerpart_as_float(self, serial_no):
        return llbc.inl.Packet_GetHeaderPartAsDouble(self._packet_cobj, serial_no)

    def get_headerpart_as_str(self, serial_no):
        return llbc.inl.Packet_GetHeaderPartAsStr(self._packet_cobj, serial_no)

    def __str__(self):
        return self._str

llbc.Packet = pyllbcPacket

