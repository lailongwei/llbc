# -*- coding: utf-8 -*-

import llbc
import traceback

class pyllbcServiceEvent(object):
    """
    The llbc service event encapsulation.
    """
    def __init__(self, svc):
        self._svc = svc
        self._idletime = None
        self._session_id = None
        self._accept_session_id = None
        self._connected = None
        self._reason = None
        self._local_ip = None
        self._local_port = None
        self._peer_ip = None
        self._peer_port = None
        self._islisten = None
        self._socket = None
        self._packet = None
        self._report_layer = None
        self._report_level = None
        self._report_msg = None
        self._opcode = None
        self._destroyed_from_service = None
        self._errno = None
        self._sub_errno = None
        self._traceback = None
        self._exception = None

    @property
    def svc(self):
        """
        Service - service object.
        """
        return self._svc

    @property
    def idletime(self):
        """sessionId
        float - idle time, in seconds.
        """
        return self._idletime

    @property
    def session_id(self):
        """
        int - session Id.
        """
        return self._session_id

    @property
    def accept_session_id(self):
        """
        int - accept session Id.
        """
        return self._accept_session_id

    @property
    def connected(self):
        """
        bool - connected flag.
        """
        return self._connected

    @property
    def reason(self):
        """
        str - reason.
        """
        return self._reason

    @property
    def localip(self):
        """
        str - local ip address.
        """
        return self._local_ip

    @property
    def localport(self):
        """
        int - local port number.
        """
        return self._local_port

    @property
    def peerip(self):
        """
        str - peer ip address.
        """
        return self._peer_ip

    @property
    def peerport(self):
        """
        int - peer port number.
        """
        return self._peer_port

    @property
    def islisten(self):
        """
        bool - listen socket flag.
        """
        return self._islisten

    @property
    def socket(self):
        """
        int - socket file descripter.
        """
        return self._socket

    @property
    def packet(self):
        """
        Packet - packet object.
        """
        return self._packet

    @property
    def report_layer(self):
        """
        int - report layer.
        """
        return self._report_layer

    @property
    def report_level(self):
        """
        int - report level
        """
        return self._report_level

    @property
    def report_msg(self):
        """
        str - report message.
        """
        return self._report_msg

    @property
    def opcode(self):
        """
        int - packet opcode.
        """
        return self._opcode

    @property
    def destroyed_from_service(self):
        """
        bool - destroyed from service flag.
        """
        return self._destroyed_from_service

    @property
    def errno(self):
        """
        int - error number, 0 if success.
        """
        return self._errno

    @property
    def sub_errno(self):
        """
        int - sub error number, 0 if success.
        """
        return self._sub_errno

    @property
    def traceback(self):
        """
        Traceback - exception traceback.
        """
        return self._traceback

    @property
    def exception(self):
        """
        Exception - exception object.
        """
        return self._exception

    def __str__(self):
        s = 'svc:[{}]'.format(self._svc)
        if self._idletime is not None:
            s += ', idletime:{}'.format(self._idletime)
        if self._session_id is not None:
            s += ', session_id:{}'.format(self._session_id)
        if self._connected is not None:
            s += ', connected:{}'.format(self._connected)
        if self._reason is not None:
            s += ', reason:{}'.format(self._reason)
        if self._local_ip is not None:
            s += ', localaddr:{}:{}'.format(self._local_ip, self._local_port)
        if self._peer_ip is not None:
            s += ', peeraddr:{}:{}'.format(self._peer_ip, self._peer_port)
        if self._islisten is not None:
            s += ', islisten:{}'.format(self._islisten)
        if self._socket is not None:
            s += ', socket:{}'.format(self._socket)
        if self._packet is not None:
            s += ', packet:[{}]'.format(self._packet)
        if self._report_layer is not None:
            s += ', report_layer:{}'.format(llbc.Protocol.layer2str(self._report_layer))
        if self._report_level is not None:
            s += ', report_level:{}'.format(llbc.Protocol.level2str(self._report_level))
        if self._report_msg is not None:
            s += ', report_msg:{}'.format(self._report_msg)
        if self._opcode is not None:
            s += ', opcode:{}'.format(self._opcode)
        if self._destroyed_from_service is not None:
            s += ', from_service?:{}'.format(self._destroyed_from_service)
        if self._errno is not None:
            s += ', errno:{}'.format(self._errno)
        if self._sub_errno is not None:
            s += ', sub_errno:{}'.format(self._sub_errno)
        if self._traceback is not None:
            s += ", tb:{}".format(''.join(traceback.format_tb(self._traceback)))
        if self._exception is not None:
            s += ", exc_type:{}, exc:{}".format(self._exception.__class__, self._exception)

        return s

llbc.ServiceEvent = pyllbcServiceEvent

