# -*- coding: utf-8 -*-

import llbc

class pyllbcPacket(object):
    def __init__(self, svc, sender_svc_id, session_id, local_ip, local_port, peer_ip, peer_port, opcode, status, status_desc, data, extdata1, extdata2, extdata3, packet_cobj):
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
        self.extdata1 = extdata1
        self.extdata2 = extdata2
        self.extdata3 = extdata3

        self._packet_cobj = packet_cobj

        self._str = None

    @property
    def recver_svc_id(self):
        return self.svc.id

    def __str__(self):
        if self._str is None:
            d = {
                'sender_svc_id': self.sender_svc_id,
                'recver_svc_id': self.recver_svc_id,
                'session_id': self.session_id,
                'local_addr': '{}:{}'.format(self.local_ip, self.local_port),
                'peer_addr': '{}:{}'.format(self.peer_ip, self.peer_port),
                'opcode': self.opcode,
                'status': self.status,
                'status_desc': self.status_desc,
                'data': str(self.data),
                'extdata1': self.extdata1,
                'extdata2': self.extdata2,
                'extdata3': self.extdata3,
                }
            self._str = str(d)
        return self._str

llbc.Packet = pyllbcPacket

