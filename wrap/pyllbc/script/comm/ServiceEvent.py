# -*- coding: utf-8 -*-

import llbc

class pyllbcServiceEvent(object):
    """
    The llbc service event encapsulation.
    """
    def __init__(self, svc):
        self._svc = svc
        self._idletime = None
        self._session_id = None
        self._connected = None
        self._reason = None
        self._local_ip = None
        self._local_port = None
        self._peer_ip = None
        self._peer_port = None
        self._islisten = None
        self._socket = None
        self._report_layer = None
        self._report_level = None
        self._report_msg = None
        self._opcode = None

    @property
    def svc(self):
        return self._svc

    @property
    def idletime(self):
        return self._idletime

    @property
    def session_id(self):
        return self._session_id

    @property
    def connected(self):
        return self._connected

    @property
    def reason(self):
        return self._reason

    @property
    def localip(self):
        return self._local_ip

    @property
    def localport(self):
        return self._local_port

    @property
    def peerip(self):
        return self._peer_ip

    @property
    def peerport(self):
        return self._peer_port

    @property
    def islisten(self):
        return self._islisten

    @property
    def socket(self):
        return self._socket

    @property
    def report_layer(self):
        return self._report_layer

    @property
    def report_level(self):
        return self._report_level

    @property
    def report_msg(self):
        return self._report_msg

    @property
    def opcode(self):
        return self._opcode

    def __str__(self):
        s = 'svc: {}'.format(self._svc)
        if self._idletime is not None:
            s += ', idletime: {}'.format(self._idletime)
        if self._session_id is not None:
            s += ', session_id: {}'.format(self._session_id)
        if self._connected is not None:
            s += ', connected: {}'.format(self._connected)
        if self._reason is not None:
            s += ', reason: {}'.format(self._reason)
        if self._local_ip is not None:
            s += ', localaddr: {}:{}'.format(self._local_ip, self._local_port)
        if self._peer_ip is not None:
            s += ', peeraddr: {}:{}'.format(self._peer_ip, self._peer_port)
        if self._islisten is not None:
            s += ', islisten: {}'.format(self._islisten)
        if self._socket is not None:
            s += ', socket: {}'.format(self._socket)
        if self._report_layer is not None:
            s += ', report_layer: {}'.format(llbc.Protocol.layer2str(self._report_layer))
        if self._report_level is not None:
            s += ', report_level: {}'.format(llbc.Protocol.level2str(self._report_level))
        if self._report_msg is not None:
            s += ', report_msg: {}'.format(self._report_msg)
        if self._opcode is not None:
            s += ', opcode: {}'.format(self._opcode)
        return s

llbc.ServiceEvent = pyllbcServiceEvent

