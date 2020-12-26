# -*- coding: utf-8 -*-

import llbc

class pyllbcPacket(object):
    def __init__(self, svc, sender_svc_id, session_id, opcode, status, data, cobj):
        self.svc = svc
        self.sender_svc_id = sender_svc_id
        self.session_id = session_id

        self._local_ip = None
        self._local_port = None
        self._peer_ip = None
        self._peer_port = None

        self.opcode = opcode
        self.status = status
        self._status_desc = None

        self.data = data

        self._extdata1 = None
        self._extdata2 = None
        self._extdata3 = None

        self._cobj = cobj

        self._str = None

    @property
    def recver_svc_id(self):
        """receiver service Id"""
        return self.svc.id

    @property
    def local_ip(self):
        """local ip address"""
        if self._local_ip is not None:
            return self._local_ip

        self._local_ip = self._cobj != 0 and llbc.inl.Packet_GetLocalIp(self._cobj) or ''
        return self._local_ip

    @property
    def local_port(self):
        """local port"""
        if self._local_port is not None:
            return self._local_port

        self._local_port = self._cobj != 0 and llbc.inl.Packet_GetLocalPort(self._cobj) or 0
        return self._local_port

    @property
    def peer_ip(self):
        """peer ip address"""
        if self._peer_ip is not None:
            return self._peer_ip

        self._peer_ip = self._cobj != 0 and llbc.inl.Packet_GetPeerIp(self._cobj) or ''
        return self._peer_ip

    @property
    def peer_port(self):
        """peer ip port"""
        if self._peer_port is not None:
            return self._peer_port

        self._peer_port = self._cobj != 0 and llbc.inl.Packet_GetPeerPort(self._cobj) or 0
        return self._peer_port

    @property
    def status_desc(self):
        """status describe"""
        if self._status_desc is not None:
            return self._status_desc

        self._status_desc = self._cobj != 0 and llbc.inl.Packet_GetStatusDesc(self._cobj) or ''
        return self._status_desc

    @property
    def extdata1(self):
        """packet extend data1"""
        if self._extdata1 is not None:
            return self._extdata1

        self._extdata1 = self._cobj != 0 and llbc.inl.Packet_GetExtData1(self._cobj) or 0
        return self._extdata1

    @property
    def extdata2(self):
        """packet extend data2"""
        if self._extdata2 is not None:
            return self._extdata2

        self._extdata2 = self._cobj != 0 and llbc.inl.Packet_GetExtData2(self._cobj) or 0
        return self._extdata2

    @property
    def extdata3(self):
        """packet extend data3"""
        if self._extdata3 is not None:
            return self._extdata3

        self._extdata3 = self._cobj != 0 and llbc.inl.Packet_GetExtData3(self._cobj) or 0
        return self._extdata3

    def _reuse(self, _, sender_svc_id, session_id, opcode, status, data, cobj):
        """Reuse packet(internal call)"""
        self.sender_svc_id = sender_svc_id
        self.session_id = session_id

        self._local_ip = None
        self._local_port = None
        self._peer_ip = None
        self._peer_port = None
        
        self.opcode = opcode
        self.status = status
        self._status_desc = None
        
        self.data = data

        self._extdata1 = None
        self._extdata2 = None
        self._extdata3 = None

        self._cobj = cobj

        self._str = None

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

