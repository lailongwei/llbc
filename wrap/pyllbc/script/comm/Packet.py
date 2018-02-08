# -*- coding: utf-8 -*-

import llbc

class pyllbcPacket(object):
    def __init__(self, svc, sender_svc_id, session_id, local_ip, local_port, peer_ip, peer_port, opcode, status, status_desc, data, packet_cobj):
        self.svc = svc
        self.sender_svc_id = sender_svc_id
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

        self._str = 'sender_svc_id: {}, recver_svc_id: {}, session_id: {}, local: {}:{}, peer: {}:{}, opcode: {}, status: {}, status_desc: {}, data: {}'.format(
                sender_svc_id, self.recver_svc_id, session_id, local_ip, local_port, peer_ip, peer_port, opcode, status, status_desc, data)

    @property
    def recver_svc_id(self):
        return self.svc.id

    def __str__(self):
        return self._str

llbc.Packet = pyllbcPacket

