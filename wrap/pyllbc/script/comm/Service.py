# -*- coding: utf-8 -*-

import sys as _sys
import types as _types
from time import time as _pyllbc_time
from time import sleep as _pyllbc_sleep
from weakref import ref as _ref

import llbc
from llbc import Packet


class pyllbcSvcExcHandler(object):
    """
    pyllbc service exception handler class encapsulation.
    """

    # The handler type enumeration
    BeginHandlerType = 0
    Subscribe = BeginHandlerType + 0
    PreSubscribe = BeginHandlerType + 1

    InvalidateHandlerType = BeginHandlerType + 2

    @classmethod
    def type_2_str(cls, ty):
        if ty == cls.Subscribe:
            return 'Subscribe'
        elif ty == cls.PreSubscribe:
            return 'PreSubscribe'
        else:
            return 'Unknown'

    @classmethod
    def is_validate_handler_type(cls, ty):
        return cls.BeginHandlerType <= ty < cls.InvalidateHandlerType

    @property
    def real_handler(self):
        return self._handler

    def __init__(self, ty, svc, handler):
        if not self.is_validate_handler_type(ty):
            raise llbc.error('invalidate handler type: {}'.format(ty))
        elif svc is None or (handler is None or not callable(handler)):
            raise llbc.error('service[{}] invalidate or handler[{}] invalidate'.format(svc, handler))

        self._ty = ty
        self._svc = _ref(svc)
        self._handler = handler

    def __call__(self, packet):
        try:
            return self._handler(packet)
        except Exception, e:
            op = packet.opcode

            ty = self._ty
            svc = self._svc()
            if ty == self.Subscribe:
                exc_handler = svc._subscribe_exc_handlers.get(op)
                if exc_handler is None:
                    exc_handler = svc._dft_subscribe_exc_handler
            elif ty == self.PreSubscribe:
                exc_handler = svc._presubscribe_exc_handlers.get(op)
                if exc_handler is None:
                    exc_handler = svc._dft_presubscribe_exc_handler
            else:
                exc_handler = None

            tb = _sys.exc_info()[2]
            if exc_handler is not None:
                exc_handler(packet, tb, e)
                self._fire_sessionexception_ev_to_comps(packet, e, tb)
            else:
                self._fire_sessionexception_ev_to_comps(packet, e, tb)
                raise

    def __repr__(self):
        return self._handler.__repr__()

    def __str__(self):
        return self._handler.__str__()

    def to_string(self):
        handler_type = self.type_2_str(self._ty)
        return 'handler type: {}, handler: {}, registered in service: [{}]'.format(handler_type, self._handler, self._svc())

    def _fire_sessionexception_ev_to_comps(self, packet, e, tb):
        svc = self._svc()
        ev = llbc.ServiceEvent(svc)
        ev._session_id = packet.session_id
        ev._packet = packet
        ev._traceback = tb
        ev._exception = e
        for comp in svc.comps.itervalues():
            if not hasattr(comp, 'onsessionexception'):
                continue

            getattr(comp, 'onsessionexception')(ev)

llbc.inl.SvcExcHandler = pyllbcSvcExcHandler 

class pyllbcServiceMetaCls(type):
    """
    pyllbc service meta class define.
    """
    def __init__(cls, name, bases, attrs):
        super(pyllbcServiceMetaCls, cls).__init__(name, bases, attrs)

    def __getitem__(cls, item):
        return cls._svcs_dict[item]

    def __setitem__(cls, item, value):
        raise NotImplementedError('Service __setitem__ method not implemented')

    def __delitem__(cls, item):
        svc = cls[item]
        if svc.started:
            svc.stop()
        else:
            svc._will_dels.update((svc, ))

        if not cls._scheduling:
            cls._procwilldelsvcs()

    def __getattr__(cls, key):
        svc = cls._svcs_dict.get(key)
        if svc is not None:
            return svc
        else:
            return super(pyllbcServiceMetaCls, cls).__getattr__(key)

    def __setattr__(cls, key, value):
        if key in cls._svcs_dict:
            raise llbc.error('attribute [{}] already used by service name, could not override'.format(key))
        else:
            super(pyllbcServiceMetaCls, cls).__setattr__(key, value)

    def __delattr__(cls, key):
        if key in cls._svcs_dict:
            cls.__delitem__(key)
        else:
            super(pyllbcServiceMetaCls, cls).__delattr__(key)

    def __iter__(cls):
        return cls._svcs_list.__iter__()

    def __len__(cls):
        return cls._svcs_list.__len__()

    @property
    def scheduling(cls):
        return cls._scheduling

llbc.ServiceMetaClass = pyllbcServiceMetaCls

class pyllbcService(object):
    """
    pyllbc library service class encapsulation.
    """
    # set serivce meta class.
    __metaclass__ = pyllbcServiceMetaCls

    # service type define.
    NORMAL = 1 # Normalize type service.
    RAW = 2 # Raw type service.

    # FPS limit values define.
    MIN_FPS = llbc.inl.SVC_MIN_FPS
    MAX_FPS = llbc.inl.SVC_MAX_FPS

    # services startup limit
    MAX_COUNT = llbc.inl.SVC_MAX_COUNT

    # ----------------------------------------------------------------------
    # internal uses:
    # all services list & dict
    _svcs_list = []
    _svcs_dict = {}

    # service schedule helper data structures.
    _will_dels = set()
    _scheduling = False
    _descheduling = False

    # frame exception handler.
    _frame_exc_handler = None
    # ----------------------------------------------------------------------

    def __init__(self, svcname, svctype=NORMAL):
        """
        Create new service
        :param svctype: the service type, see service type enumeration.
        :param svcname: the service name, must be not empty.
        """
        if isinstance(svcname, unicode):
            svcname = svcname.encode('utf-8')

        if not isinstance(svcname, str):
            raise llbc.error('service name must str type')
        elif not svcname:
            raise llbc.error('service name must be not empty')

        if self.isgonetolimit():
            raise llbc.error('service count is gone to limit')

        self._svcname = svcname
        self._svctype = svctype

        self._addsvc(self)
        self._c_obj = llbc.inl.NewService(self, svcname, self._svctype == self.NORMAL)
        self._svcid = llbc.inl.GetServiceId(self._c_obj)

        self._encoders = {}
        self._comps = {}

        cobj = self._c_obj
        self._fps = llbc.inl.GetServiceFPS(cobj)
        self._frameinterval = llbc.inl.GetServiceFrameInterval(cobj)

        self._last_schedule_time = 0
        self._started = False

        self._terminating = False
        self._terminated = False

        self._subscribe_exc_handlers = dict()  # key: opcode, value: exception handler
        self._presubscribe_exc_handlers = dict()  # key: opcode, value: exception handler

        self._dft_subscribe_exc_handler = None 
        self._dft_presubscribe_exc_handler = None

    def __del__(self):
        if hasattr(self, '_c_obj'):
            llbc.inl.DelService(self._c_obj)

    def __str__(self):
        return 'objid:{}, type:{}, name:{}, started:{}, scheduling:{}'.format(
                id(self), self.typestr, self._svcname, self._started, self.scheduling)

    def __repr__(self):
        return "Service('{}', Service.{})".format(self.name, self.typestr)

    @property
    def type(self):
        return self._svctype

    @property
    def typestr(self):
        return 'NORMAL' if self._svctype == self.NORMAL else 'RAW'

    @property
    def id(self):
        return self._svcid

    @property
    def name(self):
        return self._svcname

    @property
    def started(self):
        return self._started

    @property
    def fps(self):
        return self._fps

    @fps.setter
    def fps(self, newfps):
        llbc.inl.SetServiceFPS(self._c_obj, newfps)

        self._fps = newfps
        self._frameinterval = llbc.inl.GetServiceFrameInterval(self._c_obj)

    @property
    def frameinterval(self):
        return self._frameinterval

    @property
    def comps(self):
        return self._comps

    def suppress_codernotfound_warning(self):
        """
        Suppress all coder not found warning.
        If suppressed, packet handler received packet.data type is llbc.Stream type.
        """
        llbc.inl.SuppressServiceCoderNotFoundWarning(self._c_obj)

    def start(self, pollercount=1):
        """
        Start service.
        :param pollercount: the poller count, default is 1.
        """
        self._startcheck()

        self._autoregist()
        if self not in self.__class__._svcs_list:
            self._addsvc(self)

        llbc.inl.StartService(self._c_obj, pollercount)
        self._started = True

    @classmethod
    def set_frame_exc_handler(cls, exc_handler):
        """
        Set service per-frame exception handler(class method).
        handler can be function or method or callable object.
        handler method proto-type:
            the_frame_exception_handler(packet, traceback_obj, exception_value)
                service_obj: the service instance, this params maybe None if raised in timer.
                traceback_obj: the traceback type instance.
                error_value: the exception value.
        """
        if exc_handler is None:
            cls._frame_exc_handler = None
        elif not callable(exc_handler):
            raise llbc.error('{} not callable'.format(exc_handler))
        else:
            cls._frame_exc_handler = exc_handler

    def stop(self):
        """
        Stop service
        """
        if not self._started:
            return None

        self._started = False
        self._terminating = True

        try:
            llbc.inl.StopService(self._c_obj)
        except BaseException, e:
            raise
        finally:
            self.__class__._will_dels.update((self, ))

    @classmethod
    def get(cls, svcname):
        """
        Get service by name
        """
        return cls._svcs_dict.get(svcname)

    @classmethod
    def isgonetolimit(cls):
        return True if len(cls._svcs_list) >= cls.MAX_COUNT else False

    @classmethod
    def schedule(cls, schedule_time=0, idle_sleep=True):
        """
        Service mainloop
        :param schedule_time: specific schedule time, in seconds, if less than or equal to 0, schedule or return immediately.
        :param idle_sleep: at one schedule operation, if service check has idle time, will sleep if idle_sleep set to True, otherwise not sleep.
        """
        if cls._scheduling:
            raise llbc.error('Service in scheduling, not allow to reschedule')
        cls._scheduling = True

        svcs = cls._svcs_list
        schedule_interval = 1.0 / cls.MAX_FPS
        svc_mainloop = llbc.inl.ServiceMainLoop
        update_timers = llbc.inl.PyTimerUpdateAllTimers
        inst_errhooker = llbc.inl.InstallErrHooker
        uninst_errhooker = llbc.inl.UninstallErrHooker
        clear_hookederrors = llbc.inl.ClearHookedErrors

        try:
            inst_errhooker()
            cls._procwilldelsvcs()

            if schedule_time > 0:
                schedule_beg = _pyllbc_time()

            while True:
                if idle_sleep:
                    scheduleonce_beg = _pyllbc_time()

                clear_hookederrors()
                try:
                    for svc in svcs:
                        if not svc._started:
                            continue
    
                        svc_beg = _pyllbc_time()
                        if svc_beg - svc._last_schedule_time >= svc._frameinterval:
                            cobj = svc._c_obj
                            try:
                                svc_mainloop(cobj)
                            except Exception, e:
                                frame_exc_handler = cls._frame_exc_handler
                                if frame_exc_handler is not None:
                                    tb = _sys.exc_info()[2]
                                    frame_exc_handler(svc, tb, e)
                                else:
                                    raise
                            finally:
                                svc._last_schedule_time = svc_beg

                        if cls._procpendingdeschedule():
                            return
                        
                    update_timers()
                    if cls._procpendingdeschedule():
                        return
                except Exception, e:
                    frame_exc_handler = cls._frame_exc_handler
                    if frame_exc_handler is not None:
                        tb = _sys.exc_info()[2]
                        frame_exc_handler(None, tb, e)
                    else:
                        raise
                finally:
                    cls._procwilldelsvcs()

                now = _pyllbc_time()
                if schedule_time > 0:
                    if now - schedule_beg >= schedule_time:
                        break
    
                if idle_sleep:
                    elapsed = now - scheduleonce_beg
                    if elapsed < 0:
                        continue
                    elif elapsed < schedule_interval:
                        _pyllbc_sleep(schedule_interval - elapsed)
        except BaseException, e:
            raise
        finally:
            uninst_errhooker()
            cls._scheduling = False

    @classmethod
    def deschedule(cls):
        if not cls._scheduling:
            return
        elif cls._descheduling:
            return

        cls._descheduling = True

    @property
    def scheduling(self):
        return self.__class__.scheduling

    def addcomp(self, comp, libpath='', libcomp_cls=None):
        """
        Add component.
            component methods(all methods are optional):
                oninit(self, ev): service initialize handler.
                    ev.svc: service object.
                ondestroy(self, ev): service destroy handler.
                    ev.svc: service object.
                onstart(self, ev): service start handler.
                    ev.svc: service object.
                onstop(self, ev): service stop handler.
                    ev.svc: service object.
                onupdate(self, ev): service per-frame update handler.
                    ev.svc: service object.
                onidle(self, ev): service per-frame idle handler.
                    ev.svc: service object.
                    ev.idletime: idle time, float type, in seconds.
                onsessioncreate(self, ev): session create handler.
                    ev.svc: service object.
                    ev.islisten: is listen session or not.
                    ev.session_id: session Id.
                    ev.local_ip: local ip address.
                    ev.local_port: local port number.
                    ev.peer_ip: peer ip address.
                    ev.peer_port: peer port number.
                onsessiondestroy(self, ev): session destroy handler.
                    ev.svc: service object.
                    ev.session_id: session Id.
                    ev.reason: destroy reason.
                    ev.destroyed_from_service: destroyed from service flag.
                    ev.local_ip: local ip address.
                    ev.local_port: local port number.
                    ev.peer_ip: peer ip address.
                    ev.peer_port: peer port number.
                    ev.socket: socket file descripter.
                    ev.errno: error number(only available when ev.destroyed_from_service is True).
                    ev.sub_errno: sub error number(only available when ev.destroyed_from_service is True).
                onasyncconnresult(self, ev): async-connect result handler.
                    ev.svc: service object.
                    ev.session_id: session Id,  whether the connection is successful or not, session Id will be assigned.
                    ev.peer_ip: peer ip address.
                    ev.peer_port: peer port number.
                    ev.connected: connected flag.
                    ev.reason: reason describe.
                onprotoreport(self, ev): protocol report.
                    ev.svc: service object.
                    ev.report_layer: which layer protocol reported.
                    ev.report_level: report event level(DEBUG, INFO, WARN, ERROR).
                    ev.report_msg: report message.
                    ev.session_id: report session_id(optional, maybe is 0).
                onunhandledpacket(self, ev): unhandled packet.
                    ev.svc: service object.
					ev.session_id: packet session Id.
                    ev.opcode: packet opcode.
					ev.packet: packet object.
                onsessionexception(self, ev): session exception.
                    ev.svc: service object.
                    ev.session_id: session Id.
                    ev.packet: packet object.
                    ev.traceback: exception traceback.
                    ev.exception: exception object.
        """
        # normalize component
        if isinstance(comp, (str, unicode)):
            if isinstance(comp, unicode):
                comp = comp.encode('utf8')

            comp = llbc.inl.AddLibComponent(self._c_obj, comp, libpath, libcomp_cls)
        else:
            llbc.inl.AddComponent(self._c_obj, comp)

        # add some common members
        comp.svc = self

        # update comp dict
        self._comps.update({comp.__class__: comp})

        return comp 

    def getcomp(self, cls):
        """
        Get component by component class.
        """
        return self._comps.get(cls)

    def addencoder(self, opcode, encoder):
        """
        Add specific opcode's encoder.
        """
        if not hasattr(encoder, 'encode') or not callable(encoder.encode):
            raise llbc.error('invalid encoder, opcode: {}, encoder: {}'.format(opcode, encoder))
        elif encoder in self._encoders:
            raise llbc.error('encoder duplicate registered, opcode: {}, encoder: {}'.format(opcode, encoder))

        self._encoders.update({encoder:opcode})

    def adddecoder(self, opcode, coder):
        """
        Add specific opcode's decoder.
        Note: must raw types(eg:int, long, float, str, bytearray, unicode...) or exist follow methods' class:
              decode(self, stream): decode data from stream.
        """

        llbc.inl.AddDecoder(self._c_obj, opcode, coder)

    def listen(self, ip, port):
        """
        Listen in specified address.
        :return: the session id.
        """
        return llbc.inl.Listen(self._c_obj, ip, port)

    def connect(self, ip, port):
        """
        Connect to peer.
        :return: the session id.
        """
        return llbc.inl.Connect(self._c_obj, ip, port)

    def asyncconn(self, ip, port):
        """
        Asynchronous connect to peer(non-blocking, direct return)
        """
        return llbc.inl.AsyncConn(self._c_obj, ip, port)

    def issessionvalidate(self, session_id):
        """
        Check given session validate or not.
        """
        return llbc.inl.IsSessionValidate(self._c_obj, session_id)

    def removesession(self, session_id, reason='', strict=False):
        """
        Remove session
        :param session_id: the will remove session Id.
        :param reason: remove reason, default is empty string.
        :param strict: strict mode flag, default is false, if set to true and the session not exist or already removed, will raise an exception.
        """
        llbc.inl.RemoveSession(self._c_obj, session_id, reason, strict)

    def send(self, session_id, data, opcode=None, status=0, extData1=0, extData2=0, extData3=0):
        """
        Send data to specific session
        """
        cls = self.__class__
        if self._svctype == cls.NORMAL:
            if opcode is None:
                opcode = self._encoders.get(data.__class__)
                if opcode is None:
                    raise llbc.error('will send data not specific opcode(forgot use "@llbc.forsend" decorator?), '
                            'data: {}, type: {}'.format(data, data.__class__))
        else:
            opcode = 0
        
        # Support using packet as session_id to send packet.
        if isinstance(session_id, Packet):
            session_id = session_id.session_id

        llbc.inl.SendData(self._c_obj, session_id, opcode, data, status, extData1, extData2, extData3)

    def multicast(self, session_ids, data, opcode=None, status=0):
        """
        Multicast message
        """
        llbc.inl.Multicast(self._c_obj, session_ids, opcode, data, status)

    def broadcast(self, data, opcode=None, status=0):
        """
        Broadcast message
        """
        llbc.inl.Broadcast(self._c_obj, opcode, data, status)

    def subscribe(self, opcode, handler, exc_handler=None):
        """
        Subscribe specific opcode's message.
          handler search order:
            1> callable search(included impl __call__() meth object, bound self method, function).
            2> handle() method search.
          handler(function/method) proto-type.
            >>>> YourCls.handle(self, packet): pass
            >>>> YourCls.__call__(self, packet): pass
            >>>> def handle(packet): pass
          packet properties describe: 
            packet.svc: service
            packet.opcode: opcode
            packet.status: status
            packet.session_id: session Id
            packet.local_ip: local ip
            packet.local_port: local port
            packet.peer_ip: peer ip
            packet.peer_port: peer port
            packet.data: <the data>
        """
        _WH = pyllbcSvcExcHandler
        wrap_handler = _WH(_WH.Subscribe, self, handler)

        opcode = llbc.inl.normalize_opcode(opcode)[0]
        llbc.inl.Subscribe(self._c_obj, opcode, wrap_handler)

        if exc_handler is not None:
            self.set_subscribe_exc_handler(opcode, exc_handler)

    def set_subscribe_exc_handler(self, opcode, exc_handler):
        """
        Set the packet handler's exception handler.
        handler proto-type:
            the_packet_exception_handler(packet, traceback_obj, exception_value)
                packet: the packet object.
                traceback_obj: the traceback type instance.
                error_value: the exception value.
        """
        opcode = llbc.inl.normalize_opcode(opcode)[0]
        if exc_handler is None:
            if opcode in self._subscribe_exc_handlers:
                del self._subscribe_exc_handlers[opcode]
        elif not callable(exc_handler):
            raise llbc.error('{} not callable'.format(exc_handler))
        else:
            self._subscribe_exc_handlers[opcode] = exc_handler

    def set_default_subscribe_exc_handler(self, dft_exc_handler):
        """
        Set the packet handler's default exception handler.
        handler proto-type:
            the_packet_exception_handler(packet, traceback_obj, exception_value)
                packet: the packet object.
                traceback_obj: the traceback type instance.
                error_value: the exception value.
        """
        if dft_exc_handler is None:
            self._dft_subscribe_exc_handler = None
        elif not callable(dft_exc_handler):
            raise llbc.error('{} not callable'.format(dft_exc_handler))
        else:
            self._dft_subscribe_exc_handler = dft_exc_handler

    def presubscribe(self, opcode, prehandler, exc_handler=None):
        """
        PreSubscribe specific opcode's message.
        For get more informations, see subscribe() method.
        """
        _WH = pyllbcSvcExcHandler
        wrap_prehandler = _WH(_WH.PreSubscribe, self, prehandler)

        opcode = llbc.inl.normalize_opcode(opcode)[0]
        llbc.inl.PreSubscribe(self._c_obj, opcode, wrap_prehandler)

        if exc_handler is not None:
            self.set_presubscribe_exc_handler(opcode, exc_handler)

    def set_presubscribe_exc_handler(self, opcode, exc_handler):
        """
        Set the packet pre-handler's exception handler.
        handler proto-type:
            the_packet_exception_handler(packet, traceback_obj, exception_value)
                packet: the packet object.
                traceback_obj: the traceback type instance.
                error_value: the exception value.
        """
        opcode = llbc.inl.normalize_opcode(opcode)[0]
        if exc_handler is None:
            if opcode in self._presubscribe_exc_handlers:
                del self._presubscribe_exc_handlers[opcode]
        elif not callable(exc_handler):
            raise llbc.error('{} not callable'.format(exc_handler))
        else:
            self._presubscribe_exc_handlers[opcode] = exc_handler

    def set_default_presubscribe_exc_handler(self, dft_exc_handler):
        """
        Set the packet pre-handler's default exception handler.
        handler proto-type:
            the_packet_exception_handler(packet, traceback_obj, exception_value)
                packet: the packet object.
                traceback_obj: the traceback type instance.
                error_value: the exception value.
        """
        if dft_exc_handler is None:
            self._dft_presubscribe_exc_handler = None
        elif not callable(dft_exc_handler):
            raise llbc.error('{} not callable'.format(dft_exc_handler))
        else:
            self._dft_presubscribe_exc_handler = dft_exc_handler

    def unify_presubscribe(self, prehandler):
        """
        Unify PreSubscribe message.
        For get more informations, see subscribe() method.
        """
        llbc.inl.UnifyPreSubscribe(self._c_obj, prehandler)

    def subscribe_event(self, evid, listener):
        """
        Subscribe event.
        :param evid: the event id, must be integer type.
        :param listener: the event listener.
        :return stub: the event stub.
        """
        return llbc.inl.SubscribeEvent(self._c_obj, evid, listener)

    def unsubscribe_event(self, stub):
        """
        Unsubscribe event(by event stub).
        :param stub: the event stub.
        :return: None
        """
        llbc.inl.UnsubscribeEventByStub(self._c_obj, stub)

    def unsubscribe_event2(self, evid):
        """
        Unsubscribe event(by event id).
        :param evid: the event id.
        :return: None
        """
        llbc.inl.UnsubscribeEventById(self._c_obj, evid)

    def fire_event(self, ev):
        """
        Fire event.
        :param ev: the event object.
        :return: None
        """
        llbc.inl.FireEvent(self._c_obj, ev)

    def post(self, cble):
        """
        Post callable object to service, service will lazy call it.
        """
        llbc.inl.Post(self._c_obj, cble)

    def _startcheck(self):
        if self._started:
            raise llbc.error('service object already started')
        elif self._terminating:
            raise llbc.error('service object terminating')

    def _autoregist(self):
        llbc.inl.SvcRegsHolder.decorate(self)
        llbc.inl.SvcRegsHolder.decorate_cls()

    @classmethod
    def _addsvc(cls, svc):
        if svc.name in cls._svcs_dict:
            raise llbc.error('service name repeat, name: {}'.format(svc.name))
        elif hasattr(cls, svc.name):
            raise llbc.error('service name is used to Service class attribute, name: {}'.format(svc.name))

        cls._svcs_list.append(svc)
        cls._svcs_dict[svc.name] = svc

    @classmethod
    def _removesvc(cls, svc):
        if svc in cls._svcs_list:
            cls._svcs_list.remove(svc)
            del cls._svcs_dict[svc.name]

    @classmethod
    def _procwilldelsvcs(cls):
        for svc in cls._will_dels:
            if svc.started:
                svc.stop()
            if svc._terminating:
                svc._terminating = False
                svc._terminated = True
            cls._removesvc(svc)

        cls._will_dels.clear()

    @classmethod
    def _procpendingdeschedule(cls):
        if cls._descheduling:
            cls._descheduling = False
            return True
        else:
            return False

llbc.Service = pyllbcService

