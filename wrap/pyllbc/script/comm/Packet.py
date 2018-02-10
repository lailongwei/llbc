# The MIT License(MIT)

# Copyright(c) 2013 lailongwei<lailongwei@126.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files(the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions :
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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

