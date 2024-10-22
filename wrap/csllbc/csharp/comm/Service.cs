// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


using System;
using System.Net;
using System.Text;
using System.Reflection;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace llbc
{
    #region ServiceType enumeration
    /// <summary>
    /// The service type enumeration.
    /// </summary>
    public enum ServiceType
    {
        /// <summary>
        /// Raw type service, it means service will not use any codec layer to encode/decode your message data,
        /// <para>all message data are what you send and what you recv.</para>
        /// </summary>
        Raw,

        /// <summary>
        /// Normal type service, with this type service, will use codec layer to encode/decode your message data.
        /// </summary>
        Normal,

        /// <summary>
        /// Custom type service, with this type service, you must specific your protocol factory to service.
        /// </summary>
        Custom,
    }
    #endregion

    #region ProtoLayer enumeration
    /// <summary>
    /// The service protocol-stack layer enumeration.
    /// </summary>
    public enum ProtoLayer
    {
        /// <summary>
        /// Pack/Unpack layer, be responsible for raw bytestream and packet conversion.
        /// </summary>
        PackLayer,

        /// <summary>
        /// Compress/DeCompress layer, be responsible for comp/decomp packet.
        /// </summary>
        CompressLayer,

        /// <summary>
        /// Encode/Decode layer, be responsible for encode/decode packet.
        /// </summary>
        CodecLayer,
    }
    #endregion

    #region ProtoReportLevel enumeration
    /// <summary>
    /// The service prorocol-stack report level enumeration.
    /// </summary>
    public enum ProtoReportLevel
    {
        Debug,
        Info,
        Warn,
        Error,
    }
    #endregion

    #region DriveMode enumeration
    /// <summary>
    /// The service drive mode enumeration.
    /// </summary>
    public enum ServiceDriveMode
    {
        SelfDrive,
        ExternalDrive,
    }
    #endregion

    #region PacketHandlePhase enumeration
    /// <summary>
    /// The packet handler handle packet phases enumeration.
    /// </summary>
    public enum PacketHandlePhase
    {
        UnifyPreHandle, // Packet in unify pre-handling.
        PreHandle, // Packet in pre-handling.
        Handle, // Packet in handling.
    }
    #endregion

    #region Packet handler/exception_handler delegates
    #region Packet handlers
    /// <summary>
    /// The packet handler delegate encapsulation.
    /// </summary>
    /// <param name="packet"></param>
    public delegate void PacketHandler(Packet packet);

    /// <summary>
    /// The packet pre-handler delegate encapsulation.
    /// </summary>
    /// <param name="packet"></param>
    public delegate bool PacketPreHandler(Packet packet);

    /// <summary>
    /// The packet unify pre-handler delegate encapsulation.
    /// </summary>
    /// <param name="packet"></param>
    public delegate bool PacketUnifyPreHandler(Packet packet);
    #endregion

    #region Packet exception handler
    /// <summary>
    /// The packet exception information class encapsulation.
    /// </summary>
    public class PacketExceptionInfo
    {
        public Packet packet;
        public PacketHandlePhase phase;
        public Exception exception;
    }

    /// <summary>
    /// The packet exception handler.
    /// </summary>
    /// <param name="exceptionInfo">the packet exception info</param>
    public delegate void PacketExcHandler(PacketExceptionInfo exceptionInfo);
    #endregion

    #region Frame exception handler
    /// <summary>
    /// Service frame exception handler.
    /// </summary>
    /// <param name="svc">which service</param>
    /// <param name="e">exception object</param>
    public delegate void FrameExceptionHandler(Service svc, Exception e);
    #endregion
    #endregion

    #region Service attributes
    /// <summary>
    /// Bind to attribute, use in IComponent/ICoder/IGlobalCoder subclass, indicate decorated class bind to which services.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = true, Inherited = false)]
    public class BindToAttribute : Attribute
    {
        /// <summary>
        /// Create BindTo attribute to bind to services which you want.
        /// </summary>
        /// <param name="svcNames">service names, canbe empty</param>
        public BindToAttribute(params string[] svcNames)
        {
            if (svcNames.Length > 0)
            {
                _svcNames = new List<string>(svcNames.Length);
                _svcNames.AddRange(svcNames);
            }
        }

        public List<string> svcNames
        {
            get { return _svcNames; }
        }

        private List<string> _svcNames;
    }

    /// <summary>
    /// Opcode coder attribute, be decorated's class will become opcode coder.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
    public class CoderAttribute : Attribute
    {
        public CoderAttribute()
        {
            _autoGen = true;
        }

        /// <summary>
        /// Let class become coder.
        /// </summary>
        /// <param name="opcode">opcode</param>
        public CoderAttribute(int opcode)
        {
            _opcode = opcode;
        }

        public bool autoGen
        {
            get { return _autoGen; }
        }

        public int opcode
        {
            get { return _opcode; }
        }

        private int _opcode;
        private bool _autoGen;
    }
    
    /// <summary>
    /// Packet handler attribute, only can'be use in IComponent's methods, if decorated, method will became packet handler.
    /// <para>method declaration must accrod with PacketHandler delegate define</para>
    /// </summary>
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = false)]
    public class HandlerAttribute : Attribute
    {
        /// <summary>
        /// Will handler packet's opcode.
        /// </summary>
        /// <param name="opcode"></param>
        public HandlerAttribute(int opcode)
        {
            _opcode = opcode;
        }

        public HandlerAttribute(Type coder)
        {
            _opcode = RegHolderCollector.GetCoderOpcode(coder);
        }

        public int opcode
        {
            get { return _opcode; }
        }

        private int _opcode;
    }

    /// <summary>
    /// Packet pre-handler attribute, only can'be use in IComponent's methods, if decorated, method will became packet pre-handler.
    /// <para>method declaration must accrod with PacketPreHandler delegate define</para>
    /// </summary>
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = false)]
    public class PreHandlerAttribute : Attribute
    {
        /// <summary>
        /// Will pre-handler packet's opcode.
        /// </summary>
        /// <param name="opcode"></param>
        public PreHandlerAttribute(int opcode)
        {
            _opcode = opcode;
        }

        public PreHandlerAttribute(Type coder)
        {
            _opcode = RegHolderCollector.GetCoderOpcode(coder);
        }

        public int opcode
        {
            get { return _opcode; }
        }

        private int _opcode;
    }

    /// <summary>
    /// Packet unify pre-handler attribute, only can'be use in IComponent's methods, if decorated, method will became packet unify pre-handler.
    /// <para>method declaration must accrod with PacketUnifyPreHandler delegate define</para>
    /// </summary>
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = false)]
    public class UnifyPreHandlerAttribute : Attribute
    {
    }

    /// <summary>
    /// Packet exception handler attribute, use to decorate all packet exception handlers(included all phases and default/non-default exception handler).
    /// </summary>
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = true, Inherited = false)]
    public class PacketExcHandlerAttribute : Attribute
    {
        /// <summary>
        /// Make be decorated handler become default packet exception handler.
        /// </summary>
        /// <param name="phase"></param>
        public PacketExcHandlerAttribute(PacketHandlePhase phase)
        {
            _asDefault = true;
            _phase = phase;
        }

        /// <summary>
        /// Make be decorated handler become non-default packet exception handler.
        /// </summary>
        /// <param name="phase"></param>
        /// <param name="opcodes"></param>
        public PacketExcHandlerAttribute(PacketHandlePhase phase, params int[] opcodes)
        {
            _phase = phase;
            if (opcodes.Length == 0 || 
                phase == PacketHandlePhase.UnifyPreHandle)
            {
                _asDefault = true;
                return;
            }

            _phase = phase;
            _opcodes = new List<int>(opcodes.Length);
            _opcodes.AddRange(opcodes);
        }

        /// <summary>
        /// Make be decorated handler become non-default packet exception handler.
        /// </summary>
        /// <param name="phase"></param>
        /// <param name="coders"></param>
        public PacketExcHandlerAttribute(PacketHandlePhase phase, params Type[] coders)
        {
            _phase = phase;
            if (coders.Length == 0 ||
                phase == PacketHandlePhase.UnifyPreHandle)
            {
                _asDefault = false;
                return;
            }

            _opcodes = new List<int>();
            for (int i = 0; i < coders.Length; ++i)
            {
                int opcode = RegHolderCollector.GetCoderOpcode(coders[i]);
                if (_opcodes.Contains(opcode))
                    continue;

                _opcodes.Add(opcode);
            }
        }

        public bool asDefault
        {
            get { return _asDefault; }
        }

        public List<int> opcodes
        {
            get { return _opcodes; }
        }

        public PacketHandlePhase phase
        {
            get { return _phase; }
        }

        private bool _asDefault;
        private List<int> _opcodes;
        private PacketHandlePhase _phase;
    }

    /// <summary>
    /// Packet frame exception handler attribute.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Method, AllowMultiple = false, Inherited = true)]
    public class FrameExcHandlerAttribute : Attribute
    {
    }
    #endregion

    #region Service
    /// <summary>
    /// The service class enumeration.
    /// </summary>
    public class Service : IDisposable
    {
        #region Service search
        /// <summary>
        /// Get service by service Id.
        /// </summary>
        /// <param name="svcId">the service Id</param>
        /// <returns>service</returns>
        public static Service Get(int svcId)
        {
            Service svc;
            lock (_gblLock)
            {
                _svcId2Svcs.TryGetValue(svcId, out svc);
            }

            return svc;
        }

        /// <summary>
        /// Get services by service name.
        /// </summary>
        /// <param name="svcName">the service name</param>
        /// <returns>service</returns>
        public static Service Get(string svcName)
        {
            Service svc;
            lock (_gblLock)
            {
                _svcName2Svcs.TryGetValue(svcName, out svc);
            }

            return svc;
        }
        #endregion

        #region Ctor/Dtor
        /// <summary>
        ///  Construct service object with specific ServiceType.
        /// </summary>
        /// <param name="name">service name</param>
        /// <param name="svcType">service type enumeration</param>
        /// <param name="fullStack">full-stack option</param>
        public Service(string name, ServiceType svcType = ServiceType.Normal, bool fullStack = true)
        {
            lock (_gblLock)
            {
                if (string.IsNullOrEmpty(name))
                    throw new LLBCException("Service name invalid");
                else if (_svcName2Svcs.ContainsKey(name))
                    throw new LLBCException("Service name repeat");

                _svcName = name;
                _svcType = svcType;
                _fullStack = fullStack;

                // Create native service.
                _nativeComp = new NativeComponent(this);
                _nativeCompDelegates = new NativeComponentDelegates(_nativeComp);

                IntPtr nativeSvcName = LibUtil.CreateNativeStr(_svcName);
                _llbcSvc = LLBCNative.csllbc_Service_Create(nativeSvcName,
                                                            svcType == ServiceType.Normal,
                                                            fullStack,
                                                            _nativeCompDelegates.svcEncodePacket,
                                                            _nativeCompDelegates.svcDecodePacket,
                                                            _nativeCompDelegates.svcPacketHandler,
                                                            _nativeCompDelegates.svcPacketPreHandler,
                                                            _nativeCompDelegates.svcPacketUnifyPreHandler,
                                                            _nativeCompDelegates.svcNativeCouldNotFoundDecoderReport);
                LibUtil.FreeNativePtr(ref nativeSvcName);
                if (_llbcSvc.ToInt64() == 0)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();

                // Add native component.
                if (LLBCNative.csllbc_Service_AddComponent(_llbcSvc,
                                                           _nativeCompDelegates.onInit,
                                                           _nativeCompDelegates.onDestroy,
                                                           _nativeCompDelegates.onStart,
                                                           _nativeCompDelegates.onStop,
                                                           _nativeCompDelegates.onUpdate,
                                                           _nativeCompDelegates.onIdle,
                                                           _nativeCompDelegates.onSessionCreate,
                                                           _nativeCompDelegates.onSessionDestroy,
                                                           _nativeCompDelegates.onAsyncConnResult,
                                                           _nativeCompDelegates.onProtoReport,
                                                           _nativeCompDelegates.onUnHandledPacket) != LLBCNative.LLBC_OK)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();

                // Add new service to global svc dictionaries.
                _svcId2Svcs.Add(LLBCNative.csllbc_Service_GetId(_llbcSvc), this);
                _svcName2Svcs.Add(name, this);

                // Register all already hold register informations to service.
                RegHolderCollector.RegisterToService(this);
            }
        }

        /// <summary>
        /// Destruct service object.
        /// </summary>
        ~Service()
        {
            Dispose(false);
        }
        #endregion

        #region properties
        #region svcType, svcName, fullStack, isStarted
        /// <summary>
        /// Get the service type.
        /// </summary>
        public ServiceType svcType
        {
            get { return _svcType; }
        }

        public int svcId
        {
            get 
            {
                lock (_lock)
                {
                    return LLBCNative.csllbc_Service_GetId(_llbcSvc);
                }
            }
        }

        /// <summary>
        /// Get the service name.
        /// </summary>
        public string svcName
        {
            get { return _svcName; }
        }

        /// <summary>
        /// Get the service full-stack option.
        /// </summary>
        public bool fullStack
        {
            get { return _fullStack; }
        }

        /// <summary>
        /// Check service started or not.
        /// </summary>
        public bool isStarted
        {
            get
            {
                lock (_lock)
                {
                    return LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0;
                }
            }
        }
        #endregion

        #region fps, frameInterval
        /// <summary>
        ///  Get/Set service fps.
        ///  <para>service fps is limit to LibConfig.commMaxServiceFPS, if greater then this config value, will raise exception</para>
        /// </summary>
        public int fps
        {
            get
            {
                lock (_lock)
                {
                    return LLBCNative.csllbc_Service_GetFPS(_llbcSvc);
                }
            }

            set
            {
                lock (_lock)
                {
                    if (LLBCNative.csllbc_Service_SetFPS(_llbcSvc, value) != LLBCNative.LLBC_OK)
                        throw ExceptionUtil.CreateExceptionFromCoreLib();
                }
            }
        }

        /// <summary>
        /// Get service frame-interval, in milli-seconds.
        /// </summary>
        public int frameInterval
        {
            get 
            {
                lock (_lock)
                {
                    return LLBCNative.csllbc_Service_GetFrameInterval(_llbcSvc);
                }
            }
        }
        #endregion

        #region driveMode
        /// <summary>
        /// Get/Set service mode.
        /// </summary>
        public ServiceDriveMode driveMode
        {
            get
            {
                lock (_lock)
                {
                    return (ServiceDriveMode)LLBCNative.csllbc_Service_GetDriveMode(_llbcSvc);
                }
            }
            set
            {
                lock (_lock)
                {
                    if (LLBCNative.csllbc_Service_SetDriveMode(_llbcSvc, (int)value) != LLBCNative.LLBC_OK)
                        throw ExceptionUtil.CreateExceptionFromCoreLib();
                }
            }
        }
        #endregion
        #endregion

        #region Start/Stop
        /// <summary>
        /// Start service.
        /// </summary>
        /// <param name="pollerCount">poller thread count</param>
        public void Start(int pollerCount = 1)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_Start(_llbcSvc, pollerCount) != 0)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();
            }
        }

        /// <summary>
        /// Stop service.
        /// </summary>
        public void Stop()
        {
            lock (_lock)
            {
                // Reset _maxPacketId.
                if (_fullStack)
                    Interlocked.Exchange(ref _maxPacketId, 0);

                // Stop native llbc service.
                LLBCNative.csllbc_Service_Stop(_llbcSvc);

                // Cleanup all buffered datas in InlComponent.
                _nativeComp.CleanupAllBufferedDatas();
            }
        }
        #endregion

        #region SetFrameExceptionHandler
        public void SetFrameExceptionHandler(FrameExceptionHandler excHandler)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException(
                        "Service '{0}' started, could not set frame exception handler", svcName);
                else if (_frameExcHandler != null)
                    throw new LLBCException(
                        "Could not repeat to set frame exception handler in service '{0}'", svcName);

                _frameExcHandler = excHandler;
            }
        }
        #endregion

        #region AddCoder, RegisterGlobalCoder
        /// <summary>
        /// Register coder, opcode and coder must be unique.
        /// </summary>
        /// <param name="opcode">the coders opcode</param>
        /// <param name="coder">the coder type</param>
        public void AddCoder(int opcode, Type coder)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Could not register coder when service running!");
                else if (_coders.ContainsKey(coder) || _coders2.ContainsKey(opcode))
                    throw new LLBCException("Could not repeat to register coder: {0}", coder);
                else if (LLBCNative.csllbc_Service_AddCoder(_llbcSvc, opcode) != LLBCNative.LLBC_OK)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();

                _CoderInfo info = new _CoderInfo();
                info.opcode = opcode;
                info.coder = coder;
                info.isICoder = coder.GetInterface(typeof(ICoder).Name, false) != null;

                _coders.Add(coder, info);
                _coders2.Add(opcode, info);
            }
        }

        /// <summary>
        /// Register global coder.
        /// </summary>
        /// <param name="globalCoder">the global coder</param>
        public void RegisterGlobalCoder(IGlobalCoder globalCoder)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Could not set global coder when service running!");

                _globalCoder = globalCoder;
            }
        }
        #endregion

        #region Listen, Connect/AsyncConn, RemoveSession, IsSessionValidate
        /// <summary>
        /// Listen in specified ip endpoint.
        /// </summary>
        /// <param name="endPoint">ip endpoint</param>
        /// <returns>new session Id</returns>
        public int Listen(IPEndPoint endPoint)
        {
            return Listen(endPoint.Address.ToString(), endPoint.Port);
        }

        /// <summary>
        /// Listen in specified host and port.
        /// </summary>
        /// <param name="host">hosten</param>
        /// <param name="port">port number</param>
        /// <returns>new session Id</returns>
        public int Listen(string host, int port)
        {
            byte[] hostBytes = Encoding.UTF8.GetBytes(host);
            lock (_lock)
            {
                unsafe
                {
                    fixed (byte* ptr = &hostBytes[0])
                    {
                        int sessionId = LLBCNative.csllbc_Service_Listen(_llbcSvc, new IntPtr(ptr), port);
                        if (sessionId == 0)
                            throw ExceptionUtil.CreateExceptionFromCoreLib();

                        return sessionId;
                    }
                }
            }
        }

        /// <summary>
        /// Connect to remote ip endpoint.
        /// </summary>
        /// <param name="endPoint">remote endpoint</param>
        /// <returns>new session Id</returns>
        public int Connect(IPEndPoint endPoint)
        {
            return Connect(endPoint.Address.ToString(), endPoint.Port);
        }

        /// <summary>
        /// Connect to remote host and port.
        /// </summary>
        /// <param name="host">remote hosten</param>
        /// <param name="port">remote port number</param>
        /// <returns>new session Id</returns>
        public int Connect(string host, int port)
        {
            byte[] hostBytes = Encoding.UTF8.GetBytes(host);
            lock (_lock)
            {
                unsafe
                {
                    fixed (byte* ptr = &hostBytes[0])
                    {
                        int sessionId = LLBCNative.csllbc_Service_Connect(_llbcSvc, new IntPtr(ptr), port);
                        if (sessionId == 0)
                            throw ExceptionUtil.CreateExceptionFromCoreLib();

                        return sessionId;
                    }
                }
            }
        }

        /// <summary>
        /// Async-Connect to remote endpoint.
        /// </summary>
        /// <param name="endPoint">remote endpoint</param>
        public void AsyncConn(IPEndPoint endPoint)
        {
            AsyncConn(endPoint.Address.ToString(), endPoint.Port);
        }

        /// <summary>
        /// Async-Connect to remote hosten and port.
        /// </summary>
        /// <param name="host">remote hosten</param>
        /// <param name="port">remote port number</param>
        public void AsyncConn(string host, int port)
        {
            byte[] hostBytes = Encoding.UTF8.GetBytes(host);
            lock (_lock)
            {
                unsafe
                {
                    fixed (byte* ptr = &hostBytes[0])
                    {
                        if (LLBCNative.csllbc_Service_AsyncConn(
                                _llbcSvc, new IntPtr(ptr), port) != LLBCNative.LLBC_OK)
                            throw ExceptionUtil.CreateExceptionFromCoreLib();
                    }
                }
            }
        }

        /// <summary>
        /// Remove session.
        /// </summary>
        /// <param name="sessionId">will remove session Id</param>
        /// <param name="reason">remove reason</param>
        /// <param name="strict">strict mode flag, if set, any remove-session error will raise exception</param>
        public void RemoveSession(int sessionId, string reason = null, bool strict = false)
        {
            int removeRet = LLBCNative.LLBC_FAILED;
            if (string.IsNullOrEmpty(reason))
            {
                lock (_lock)
                {
                    removeRet = LLBCNative.csllbc_Service_RemoveSession(_llbcSvc, sessionId, IntPtr.Zero, 0);
                }
            }
            else
            {
                byte[] reasonBytes = Encoding.UTF8.GetBytes(reason);
                lock (_lock)
                {
                    unsafe
                    {
                        fixed (byte* ptr = &reasonBytes[0])
                        {
                            removeRet = LLBCNative.csllbc_Service_RemoveSession(_llbcSvc, sessionId, new IntPtr(ptr), reasonBytes.Length);
                        }
                    }
                }
            }

            if (removeRet != LLBCNative.LLBC_OK && strict)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }

        /// <summary>
        /// Check given session Id is validate or not.
        /// </summary>
        /// <param name="sessionId">the session Id</param>
        /// <returns>true if session Id validate, otherwise return false</returns>
        public bool IsSessionValidate(int sessionId)
        {
            lock (_lock)
            {
                return LLBCNative.csllbc_Service_IsSessionValidate(_llbcSvc, sessionId) != 0;
            }
        }
        #endregion

        #region AddComponent
        /// <summary>
        /// Add new component to service.
        /// </summary>
        /// <param name="component">the new component object</param>
        public void AddComponent(IComponent component)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Could not register component when service running");

                _nativeComp.AddComponent(component);
            }
        }
        #endregion

        #region Subscribe/PreSubscribe/UnifyPreSubscribe
        /// <summary>
        /// Subscribe specific opcode's packet.
        /// </summary>
        /// <param name="opcode">opcode</param>
        /// <param name="handler">the packet handler</param>
        /// <param name="excHandler">packet exception handler</param>
        public void Subscribe(int opcode, PacketHandler handler, PacketExcHandler excHandler = null)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Service '{0}' started, could not subscribe {1}", svcName, opcode);
                else if (_handlers.ContainsKey(opcode))
                    throw new LLBCException("Could not repeat subscribe packet {0} in service {1}", opcode, svcName);

                if (LLBCNative.csllbc_Service_Subscribe(_llbcSvc, opcode) != LLBCNative.LLBC_OK)
                    throw ExceptionUtil.CreateExceptionFromCoreLib(); 

                _handlers.Add(opcode, handler);
                if (excHandler != null)
                    _excHandlers.Add(opcode, excHandler);
            }
        }

        /// <summary>
        /// PreSubscribe specific opcode's packet.
        /// </summary>
        /// <param name="opcode">opcode</param>
        /// <param name="preHandler">the packet prehandler</param>
        /// <param name="excHandler">the exception handler</param>
        public void PreSubscribe(int opcode, PacketPreHandler preHandler, PacketExcHandler excHandler = null)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Service '{0}' started, could not presubscribe {1}", svcName, opcode);
                else if (_preHandlers.ContainsKey(opcode))
                    throw new LLBCException("Could not repeat presubscribe packet {0} in service {1}", opcode, svcName);

                if (LLBCNative.csllbc_Service_PreSubscribe(_llbcSvc, opcode) != LLBCNative.LLBC_OK)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();

                _preHandlers.Add(opcode, preHandler);
                if (excHandler != null)
                    _preExcHandlers.Add(opcode, excHandler);
            }
        }

        /// <summary>
        /// Unify presubscribe packet, need LibConfig.commIsEnabledUnifyPreSubscribe == true support, if not enabled this option will raise exception.
        /// </summary>
        /// <param name="unifyPreHandler">the packet unify prehandler</param>
        /// <param name="excHandler">the exception handler</param>
        public void UnifyPreSubscribe(PacketUnifyPreHandler unifyPreHandler, PacketExcHandler excHandler = null)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Service '{0}' started, could not unify presubscribe", svcName);
                else if (_unifyPreHandler != null)
                    throw new LLBCException("Could not repeat unify presubscribe packet in service {0}", svcName);

                if (LLBCNative.csllbc_Service_UnifyPreSubscribe(_llbcSvc) != LLBCNative.LLBC_OK)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();

                _unifyPreHandler = unifyPreHandler;

                if (excHandler != null)
                    SetDftPacketExcHandler(PacketHandlePhase.UnifyPreHandle, excHandler);
            }
        }
        #endregion

        #region SetPacketExcHandler/SetDftPacketExcHandler
        /// <summary>
        /// Set packet exception handler, if in UnifyPreHandle phase, the opcode parameter will ignore.
        /// </summary>
        /// <param name="handlePhase">packet handle phase</param>
        /// <param name="excHandler">packet exception handler</param>
        /// <param name="opcode">packet opcode, </param>
        public void SetPacketExcHandler(PacketHandlePhase handlePhase, PacketExcHandler excHandler, int opcode)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException("Service '{0}' started, could not set packet exception handler", svcName);
                else if (excHandler == null)
                    throw new LLBCException("Exception handler could not be null");

                if (handlePhase == PacketHandlePhase.Handle)
                {
                    if (_excHandlers.ContainsKey(opcode))
                        throw new LLBCException("Could not repeat set service '{0}' packet exception handler, opcode:{£±}, phase: {2}", 
                            svcName, opcode, handlePhase);
                    _excHandlers.Add(opcode, excHandler);
                }
                else if (handlePhase == PacketHandlePhase.PreHandle)
                {
                    if (_preExcHandlers.ContainsKey(opcode))
                        throw new LLBCException("Could not repeat set service '{0}' packet exception handler, opcode:{£±}, phase: {2}",
                            svcName, opcode, handlePhase);
                    _preExcHandlers.Add(opcode, excHandler);
                }
                else
                {
                    if (_unifyExcHandler != null)
                        throw new LLBCException(
                            "Could not repeat set service '{0}' packet exception handler, phase: {1}", svcName, handlePhase);
                    _unifyExcHandler = excHandler;
                }
            }
        }

        /// <summary>
        /// Set paccket default exception handler.
        /// </summary>
        /// <param name="handlePhase">default exception set to packet handle phase</param>
        /// <param name="dftExcHandler">the default packet exception handler</param>
        public void SetDftPacketExcHandler(PacketHandlePhase handlePhase, PacketExcHandler dftExcHandler)
        {
            lock (_lock)
            {
                if (LLBCNative.csllbc_Service_IsStarted(_llbcSvc) != 0)
                    throw new LLBCException(
                        "Service '{0}' started, could not set default packet exception handler", svcName);
                else if (dftExcHandler == null)
                    throw new LLBCException("Exception handler could not be null");

                if (handlePhase == PacketHandlePhase.UnifyPreHandle)
                {
                    if (_unifyExcHandler != null)
                        throw new LLBCException(
                            "Could not repeat set service '{0}' default packet exception handler, phase: {1}", svcName, handlePhase);
                    _unifyExcHandler = dftExcHandler;
                }
                else if (handlePhase == PacketHandlePhase.PreHandle)
                {
                    if (_dftPreExcHandler != null)
                        throw new LLBCException(
                            "Could not repeat set service '{0}' default packet exception handler, phase: {1}", svcName, handlePhase);
                    _dftPreExcHandler = dftExcHandler;
                }
                else
                {
                    if (_dftExcHandler != null)
                        throw new LLBCException(
                            "Could not repeat set service '{0}' default packet exception handler, phase: {1}", svcName, handlePhase);
                    _dftExcHandler = dftExcHandler;
                }
            }
        }
        #endregion

        #region Send/Multicast/Broadcast
        /// <summary>
        /// Send data.
        /// </summary>
        /// <typeparam name="T">the will send data type</typeparam>
        /// <param name="sessionId">the session Id</param>
        /// <param name="obj">data object, must be class</param>
        /// <param name="status">the status</param>
        public void Send<T>(int sessionId, T obj, int status = 0) where T : class
        {
            _CoderInfo coderInfo;
            ICoder iCoder = obj as ICoder;
            if (iCoder == null) // Will send obj's type is not implemented ICoder interface, use IGlobalCoder to encode it.
            {
                // Get coder info.
                IGlobalCoder globalCoder;
                lock (_lock)
                {
                    // Check global coder.
                    if (_globalCoder == null)
                        throw new LLBCException(
                            "GlobalCoder not found, send packet[{0}] failed, sessionId:{1}", typeof(T), sessionId);
                    // Get opcode.
                    else if (!_coders.TryGetValue(typeof(T), out coderInfo))
                        throw new LLBCException("Could not find packet[{0}] opcode, please AddCoder first!", typeof(T));

                    // Enabled full-stack option, push and waiting native coder to encode it.
                    if (_fullStack)
                    {
                        _FullStackSendNonLock(sessionId, obj, coderInfo, status);
                        return;
                    }

                    globalCoder = _globalCoder;
                }

                // full-stack option disabled, encode packet and send.
                MemoryStream stream = new MemoryStream();
                if (!globalCoder.Encode(stream, obj))
                    throw new LLBCException("Encode object '{0}' failed", typeof(T));

                Send(sessionId, coderInfo.opcode, stream, status, true);
            }
            else // Implemented ICoder interface, use self.Encode method to encode.
            {
                // Get coder info.
                lock (_lock)
                {
                    if (!_coders.TryGetValue(typeof(T), out coderInfo))
                        throw new LLBCException("Could not find packet[{0}] opcode, send failed, please AddCoder first!", obj.GetType());

                    // Enabled full-stack option, push and waiting native coder to encode it.
                    if (_fullStack)
                    {
                        _FullStackSendNonLock(sessionId, obj, coderInfo, status);
                        return;
                    }
                }

                // full-stack option disabled, encode packet and send.
                MemoryStream stream = new MemoryStream();
                if (!iCoder.Encode(stream))
                    throw new LLBCException("Encode object '{0}' failed", typeof(T));

                Send(sessionId, coderInfo.opcode, stream, status, true);
            }
        }

        /// <summary>
        /// Send stream bytes.
        /// </summary>
        /// <param name="sessionId">sessionId</param>
        /// <param name="opcode">packet opcode</param>
        /// <param name="stream">stream</param>
        /// <param name="status">packet status</param>
        /// <param name="publiclyVisible">stream publicly visible flag, default is false</param>
        public void Send(int sessionId, int opcode, MemoryStream stream, int status = 0, bool publiclyVisible = false)
        {
            int len = (int)stream.Position;
            lock (_lock)
            {
                if (len == 0)
                {
                    if (LLBCNative.csllbc_Service_SendBytes(
                            _llbcSvc, sessionId, opcode, IntPtr.Zero, 0, status) != LLBCNative.LLBC_OK)
                        throw ExceptionUtil.CreateExceptionFromCoreLib();

                    return;
                }

                unsafe
                {
                    byte[] bytes = publiclyVisible ? stream.GetBuffer() : stream.ToArray();
                    fixed (byte* ptr = &bytes[0])
                    {
                        if (LLBCNative.csllbc_Service_SendBytes(
                                _llbcSvc, sessionId, opcode, new IntPtr(ptr), len, status) != LLBCNative.LLBC_OK)
                            throw ExceptionUtil.CreateExceptionFromCoreLib();
                    }
                }
            }
        }

        /// <summary>
        /// Multicast data.
        /// </summary>
        /// <typeparam name="T">the will send data type</typeparam>
        /// <param name="sessionIds">the session Ids</param>
        /// <param name="obj">data object, must be class</param>
        /// <param name="status">the status</param>
        public void Multicast<T>(List<int> sessionIds, T obj, int status = 0) where T: class
        {
            ICoder iCoder = obj as ICoder;

            _CoderInfo coderInfo;
            if (iCoder == null)
            {
                IGlobalCoder globalCoder;
                lock (_lock)
                {
                    if (_globalCoder == null)
                        throw new LLBCException(
                            "GlobalCoder not found, multicast packet[{0}] failed", obj.GetType());

                    if (!_coders.TryGetValue(typeof(T), out coderInfo))
                        throw new LLBCException("Could not find packet[{0}] opcode, multicast failed, please AddCoder first!", typeof(T));
                    globalCoder = _globalCoder;
                }

                MemoryStream stream = new MemoryStream();
                if (!globalCoder.Encode(stream, obj))
                    throw new LLBCException("Encode object '{0}' failed", typeof(T));
                    

                Multicast2(sessionIds, coderInfo.opcode, stream, status, true);
            }
            else
            {
                lock (_lock)
                {
                    if (!_coders.TryGetValue(typeof(T), out coderInfo))
                        throw new LLBCException("Could not find packet[{0}] opcode, multicast failed, please AddCoder first!", typeof(T));
                }

                MemoryStream stream = new MemoryStream();
                if (!iCoder.Encode(stream))
                    throw new LLBCException("Encode object '{0}' failed", typeof(T));

                Multicast2(sessionIds, coderInfo.opcode, stream, status, true);
            }
        }

        /// <summary>
        /// Multicast stream bytes.
        /// </summary>
        /// <param name="sessionIds">sessionIds</param>
        /// <param name="opcode">packet opcode</param>
        /// <param name="stream">stream</param>
        /// <param name="status">packet status</param>
        /// <param name="publiclyVisible">stream publicly visible flag, default is false</param>
        public void Multicast2(List<int> sessionIds, int opcode, MemoryStream stream, int status = 0, bool publiclyVisible = false)
        {
            // Copy all sessionIds to unmanaged memory area.
            IntPtr unmanagedSessionIds = sessionIds.Count == 0 ?
                IntPtr.Zero : Marshal.AllocHGlobal(sessionIds.Count * sizeof(int));
            unsafe
            {
                int* ptr = (int*)unmanagedSessionIds.ToPointer();
                for (int i = 0; i < sessionIds.Count; ++i)
                    *(ptr + i) = sessionIds[i];
            }

            int len = (int)stream.Position;
            lock (_lock)
            {
                if (len == 0)
                {
                    if (LLBCNative.csllbc_Service_Multicast(_llbcSvc,
                                                            unmanagedSessionIds,
                                                            sessionIds.Count,
                                                            opcode,
                                                            IntPtr.Zero,
                                                            0,
                                                            status) != LLBCNative.LLBC_OK)
                        throw ExceptionUtil.CreateExceptionFromCoreLib();
                    return;
                }

                unsafe
                {
                    byte[] bytes = publiclyVisible ? stream.GetBuffer() : stream.ToArray();
                    fixed (byte* ptr = &bytes[0])
                    {
                        if (LLBCNative.csllbc_Service_Multicast(_llbcSvc,
                                                                unmanagedSessionIds,
                                                                sessionIds.Count,
                                                                opcode,
                                                                new IntPtr(ptr),
                                                                len,
                                                                status) != LLBCNative.LLBC_OK)
                            throw ExceptionUtil.CreateExceptionFromCoreLib();
                    }
                }
            }
        }

        /// <summary>
        /// Broadcast data.
        /// </summary>
        /// <typeparam name="T">the will send data type</typeparam>
        /// <param name="obj">data object</param>
        /// <param name="status">the status</param>
        public void Broadcast<T>(T obj, int status = 0)
        {
            ICoder iCoder = obj as ICoder;

            _CoderInfo coderInfo;
            if (iCoder == null)
            {
                IGlobalCoder globalCoder;
                lock (_lock)
                {
                    if (_globalCoder == null)
                        throw new LLBCException(
                            "GlobalCoder not found broadcast packet[{0}] failed", typeof(T));
                    globalCoder = _globalCoder;

                    if (!_coders.TryGetValue(typeof(T), out coderInfo))
                        throw new LLBCException("Could not find packet[{0}] opcode, broadcast failed, please AddCoder first!", typeof(T));
                }

                MemoryStream stream = new MemoryStream();
                if (!globalCoder.Encode(stream, obj))
                    throw new LLBCException("Encode object '{0}' failed", typeof(T));

                Broadcast(coderInfo.opcode, stream, status, true);
            }
            else
            {
                lock (_lock)
                {
                    if (!_coders.TryGetValue(typeof(T), out coderInfo))
                        throw new LLBCException("Could not find packet[{0}] opcoce, broadcast failed, please AddCoder first!", typeof(T));
                }

                MemoryStream stream = new MemoryStream();
                if (!iCoder.Encode(stream))
                    throw new LLBCException("Encode object '{0}' failed", typeof(T));

                Broadcast(coderInfo.opcode, stream, status, true);
            }
        }

        /// <summary>
        /// Broadcast stream bytes.
        /// </summary>
        /// <param name="opcode">packet opcode</param>
        /// <param name="stream">stream</param>
        /// <param name="status">packet status</param>
        /// <param name="publiclyVisible">stream publicly visible flag, default is false</param>
        public void Broadcast(int opcode, MemoryStream stream, int status = 0, bool publiclyVisible = false)
        {
            int len = (int)stream.Position;
            lock (_lock)
            {
                if (len == 0)
                {
                    if (LLBCNative.csllbc_Service_Broadcast(_llbcSvc,
                                                            opcode,
                                                            IntPtr.Zero,
                                                            0,
                                                            status) != LLBCNative.LLBC_OK)
                        throw ExceptionUtil.CreateExceptionFromCoreLib();
                    return;
                }

                unsafe
                {
                    byte[] bytes = publiclyVisible ? stream.GetBuffer() : stream.ToArray();
                    fixed (byte* ptr = &bytes[0])
                    {
                        if (LLBCNative.csllbc_Service_Broadcast(_llbcSvc,
                                                                opcode,
                                                                new IntPtr(ptr),
                                                                len,
                                                                status) != LLBCNative.LLBC_OK)
                            throw ExceptionUtil.CreateExceptionFromCoreLib();
                    }
                }
            }
        }
        #endregion

        #region OnSvc
        /// <summary>
        /// Per-Frame service method, call when drivemode == ExternalDrive.
        /// </summary>
        /// <param name="fullFrame">full frame flag, if set to true, OnSvc() will run full frameInterval times.</param>
        public void OnSvc(bool fullFrame = false)
        {
            lock (_lock)
            {
                LLBCNative.csllbc_Service_OnSvc(_llbcSvc, fullFrame);
            }
        }
        #endregion

        #region Dispose
        /// <summary>
        /// Dispose service object.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
        }
        #endregion

        #region Internal methods
        #region Dispose impl
        virtual protected void Dispose(bool disposing)
        {
            lock (_gblLock)
            {
                lock (_lock)
                {
                    if (_disposed)
                        return;

                    // Stop service first.
                    Stop();

                    // Remove service from global dicts.
                    _svcName2Svcs.Remove(_svcName);
                    _svcId2Svcs.Remove(LLBCNative.csllbc_Service_GetId(_llbcSvc));

                    // Stop native llbc service.
                    LLBCNative.csllbc_Service_Delete(_llbcSvc);
                    _llbcSvc = IntPtr.Zero;

                    // Reset svcName,svcType
                    _svcName = null;
                    _svcType = ServiceType.Normal;

                    // Cleanup wrap-components.
                    _CleanupComps();
                    // Cleanup all coders.
                    _CleanupCoders();
                    // Cleanup all packet handlers.
                    _CleanupPacketHandlers();
                    // Cleanup frame exception handler.
                    _frameExcHandler = null;

                    if (disposing)
                        GC.SuppressFinalize(this);

                    _disposed = true;
                }
            }
        }
        #endregion

        #region Data members cleanup methods
        private void _CleanupComps()
        {
            _nativeComp = null;
            _nativeCompDelegates = null;
        }

        private void _CleanupCoders()
        {
            _coders.Clear();
            _coders2.Clear();
            _globalCoder = null;
        }

        private void _CleanupPacketHandlers()
        {
            _handlers.Clear();
            _preHandlers.Clear();
            _unifyPreHandler = null;

            _dftExcHandler = null;
            _dftPreExcHandler = null;
            _unifyExcHandler = null;
            _excHandlers.Clear();
            _preExcHandlers.Clear();
        }
        #endregion

        #region Send helper methods
        private void _FullStackSendNonLock(int sessionId, object obj, _CoderInfo coderInfo, int status)
        {
            // Check sessionId.
            if (LLBCNative.csllbc_Service_IsSessionValidate(_llbcSvc, sessionId) != 0)
                throw new LLBCException("Session Id[{0}] invalidate, send failed", sessionId);

            // Get packetId.
            long packetId = Interlocked.Increment(ref _maxPacketId);
            // Let native Service object create csllbc_Coder object to send.
            if (LLBCNative.csllbc_Service_SendPacket(_llbcSvc, sessionId, coderInfo.opcode, packetId, status) != LLBCNative.LLBC_OK)
                throw ExceptionUtil.CreateExceptionFromCoreLib();

            // Push
            _nativeComp.PushWillEncodeObj(sessionId, coderInfo.opcode, obj, coderInfo.isICoder, packetId, status);
        }
        #endregion

        #region Frame exception handle
        private void _HandleFrameExc(Exception e)
        {
            try
            {
                if (_frameExcHandler != null)
                {
                    _frameExcHandler(this, e);
                }
            }
            catch
            {
                // Keep silent.
            }
        }
        #endregion
        #endregion

        #region NativeComponent
        private class NativeComponent
        {
            /// <summary>
            /// Constructor.
            /// </summary>
            /// <param name="svc">owned service object</param>
            public NativeComponent(Service svc)
            {
                _svc = svc;
                _fullStack = svc.fullStack;
            }

            #region AddComponent/CleanupComponents
            /// <summary>
            /// Add csharp layer component.
            /// </summary>
            /// <param name="component"></param>
            public void AddComponent(IComponent component)
            {
                if (component == null)
                    throw new ArgumentException();
                else if (_comps.IndexOf(component) >= 0)
                    throw new LLBCException(
                        "Could not repeat to register component '{0}' in service: {1}", component, _svc.svcName);

                component.svc = _svc;
                _comps.Add(component);

                Type compTy = component.GetType();
                BindingFlags methBindFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.DeclaredOnly;
                if (compTy.GetMethod("OnIdle", methBindFlags) != null)
                    _overridedOnIdleComps.Add(component);
                if (compTy.GetMethod("OnUpdate", methBindFlags) != null)
                    _overridedOnUpdateComps.Add(component);
            }

            /// <summary>
            /// Cleanup all csharp layer components.
            /// </summary>
            public void CleanupComponents()
            {
                _comps.Clear();
                _overridedOnIdleComps.Clear();
                _overridedOnUpdateComps.Clear();
            }
            #endregion

            #region llbc native library component interfaces
            public void OnInit()
            {
                for (int i = 0; i < _comps.Count; ++i)
                {
                    try
                    {
                        _comps[i].OnInit();
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnDestroy()
            {
                for (int i = _comps.Count - 1; i >= 0; --i)
                {
                    try
                    {
                        _comps[i].OnDestroy();
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnStart()
            {
                for (int i = 0; i < _comps.Count; ++i)
                {
                    try
                    {
                        _comps[i].OnStart();
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnStop()
            {
                for (int i = _comps.Count - 1; i >= 0; --i)
                {
                    try
                    {
                        _comps[i].OnStop();
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnUpdate()
            {
                for (int i = 0; i < _overridedOnUpdateComps.Count; ++i)
                {
                    try
                    {
                        _overridedOnUpdateComps[i].OnUpdate();
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnIdle(int idleTime)
            {
                for (int i = 0; i < _overridedOnIdleComps.Count; ++i)
                {
                    try
                    {
                        _overridedOnIdleComps[i].OnIdle(idleTime);
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnSessionCreate(bool isListen,
                                        int sessionId,
                                        int acceptSessionId,
                                        int socketHandle,
                                        IntPtr localHost,
                                        int localHostLen,
                                        int localPort,
                                        IntPtr remoteHost,
                                        int remoteHostLen,
                                        int remotePort)
            {
                IPEndPoint localEndPoint = new IPEndPoint(
                    IPAddress.Parse(LibUtil.Ptr2Str(localHost, localHostLen)), localPort);
                IPEndPoint remoteEndPoint = new IPEndPoint(
                    IPAddress.Parse(LibUtil.Ptr2Str(remoteHost, remoteHostLen)), remotePort);

                SessionInfo sessionInfo =
                    new SessionInfo(isListen, sessionId, acceptSessionId, socketHandle, localEndPoint, remoteEndPoint);
                for (int i = 0; i < _comps.Count; ++i)
                {
                    try
                    {
                        _comps[i].OnSessionCreate(sessionInfo);
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnSessionDestroy(bool isListen,
                                         int sessionId,
                                         int acceptSessionId,
                                         int socketHandle,
                                         IntPtr localHost,
                                         int localHostLen,
                                         int localPort,
                                         IntPtr remoteHost,
                                         int remoteHostLen,
                                         int remotePort,
                                         bool fromSvc,
                                         IntPtr reason,
                                         int reasonLen,
                                         int errNo,
                                         int subErrNo)
            {
                // Cleanup sessionb about datas.
                RemoveWillSendPackets(sessionId);
                RemoveQueuedPackets(sessionId);

                // Foreach call csharp layer component.OnSessionDestroy method.
                IPEndPoint localEndPoint = new IPEndPoint(
                    IPAddress.Parse(LibUtil.Ptr2Str(localHost, localHostLen)), localPort);
                IPEndPoint remoteEndPoint = new IPEndPoint(
                    IPAddress.Parse(LibUtil.Ptr2Str(remoteHost, remoteHostLen)), remotePort);

                SessionInfo sessionInfo = new SessionInfo(
                    isListen, sessionId, acceptSessionId, socketHandle, localEndPoint, remoteEndPoint);

                string managedReason = LibUtil.Ptr2Str(reason, reasonLen);

                SessionDestroyInfo destroyInfo =
                    new SessionDestroyInfo(sessionInfo, fromSvc, managedReason, errNo, subErrNo);
                for (int i = 0; i < _comps.Count; ++i)
                {
                    try
                    {
                        _comps[i].OnSessionDestroy(destroyInfo);
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnAsyncConnResult(bool connected,
                                          IntPtr reason,
                                          int reasonLen,
                                          IntPtr remoteHost,
                                          int remoteHostLen,
                                          int remotePort)
            {
                IPEndPoint remoteEndPoint = new IPEndPoint(
                    IPAddress.Parse(LibUtil.Ptr2Str(remoteHost, remoteHostLen)), remotePort);

                string managedReason = LibUtil.Ptr2Str(reason, reasonLen);
                AsyncConnResult asyncConnResult =
                    new AsyncConnResult(connected, managedReason, remoteEndPoint);

                for (int i = 0; i < _comps.Count; ++i)
                {
                    try
                    {
                        _comps[i].OnAsyncConnResult(asyncConnResult);
                    }

                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }

            public void OnProtoReport(int sessionId,
                                      int layer,
                                      int level,
                                      IntPtr report,
                                      int reportLen)
            {
                string managedReport = LibUtil.Ptr2Str(report, reportLen);
                ProtoReport reportInfo = new ProtoReport(sessionId,
                                                         (ProtoLayer)layer,
                                                         (ProtoReportLevel)level,
                                                         managedReport);

                for (int i = 0; i < _comps.Count; ++i)
                    _comps[i].OnProtoReport(reportInfo);
            }

            public void OnUnHandledPacket(int sessionId,
                                          int opcode,
                                          IntPtr data,
                                          int dataLen,
                                          int status)
            {
                MemoryStream dataStream = new MemoryStream(dataLen);
                if (dataLen > 0)
                    Marshal.Copy(data, dataStream.GetBuffer(), 0, dataLen);

                Packet packet = new Packet(
                    _svc, sessionId, opcode, dataStream, dataLen, status, data.ToInt64());

                for (int i = 0; i < _comps.Count; ++i)
                {
                    try
                    {
                        _comps[i].OnUnHandledPacket(packet);
                    }
                    catch (Exception e)
                    {
                        _svc._HandleFrameExc(e);
                    }
                }
            }
            #endregion

            #region Encode about methods
            /// <summary>
            /// Push will encode object to wait native LLBC_Coder::Encode() call.
            /// </summary>
            public void PushWillEncodeObj(int sessionId, int opcode, object obj, bool isCoder, long packetId, int status)
            {
                Dictionary<long, _WillEncodeObjInfo> objs;

                lock (_willEncodeObjs)
                {
                    if (!_willEncodeObjs.TryGetValue(sessionId, out objs))
                    {
                        objs = new Dictionary<long, _WillEncodeObjInfo>();
                        _willEncodeObjs.Add(sessionId, objs);
                    }

                    _WillEncodeObjInfo objInfo = new _WillEncodeObjInfo();
                    objInfo.obj = obj;
                    objInfo.isCoder = isCoder;
                    objInfo.opcode = opcode;
                    objInfo.status = status;
                    objs.Add(packetId, objInfo);
                }
            }

            /// <summary>
            /// Encode unmanaged object, call by c++ native code.
            /// </summary>
            /// <param name="sessionId">session Id</param>
            /// <param name="packetId">packet Id</param>
            /// <param name="encodedLength">encoded length(out)</param>
            /// <param name="encodedSucceed">succeed flag, if non-zero means succeed, otherwise set to zero</param>
            /// <param name="errMsgLength">the encode error message length</param>
            /// <returns>encoded data, unmanaged, if encoded failed, return codec error message</returns>
            public IntPtr EncodeManagedObj(int sessionId, long packetId, IntPtr encodedSucceed, IntPtr encodedLength, IntPtr errMsgLength)
            {
                unsafe
                {
                    *(int*)encodedSucceed.ToPointer() = 0;
                }

                // Find will encode object.
                _WillEncodeObjInfo objInfo = null;
                lock (_willEncodeObjs)
                {
                    Dictionary<long, _WillEncodeObjInfo> objs;
                    if (_willEncodeObjs.TryGetValue(sessionId, out objs))
                    {
                        if (objs.TryGetValue(packetId, out objInfo))
                            objs.Remove(packetId);
                    }
                }

                // If not found will encode object info, raise error and report to native lib.
                if (objInfo == null)
                    return LibUtil.CreateNativeStr(string.Format(
                        "llbc library(c# wrap) internal error: encode managed obj failed, could not find obj to encode! sessionId: {0}, packetId: {1}", sessionId, packetId), errMsgLength);

                // Encode object.
                bool managedEncodedSucceed = false;
                MemoryStream stream = new MemoryStream();
                try
                {
                    if (objInfo.isCoder)
                        managedEncodedSucceed = (objInfo.obj as ICoder).Encode(stream);
                    else
                        managedEncodedSucceed = _svc._globalCoder.Encode(stream, objInfo.obj);
                }
                catch (Exception e)
                {
                    return LibUtil.CreateNativeStr(string.Format(
                        "Encode failed, sessionId: {0}, packet: {1}, exception:\n{2}", sessionId, objInfo.obj.GetType().Name, e), errMsgLength);
                }

                if (!managedEncodedSucceed)
                    return IntPtr.Zero;

                // Set encoded data length & mask encode success.
                int dataLen = (int)stream.Position;
                unsafe
                {
                    *(int*)encodedSucceed.ToPointer() = 1;
                    *(int*)encodedLength.ToPointer() = dataLen;
                }

                // If encoded data length is zero, direct return.
                if (dataLen == 0)
                    return IntPtr.Zero;

                // Allocate unmanaged memory and copy encoded data to unmanaged memory.
                IntPtr unmanaged = Marshal.AllocHGlobal((int)stream.Position);
                Marshal.Copy(stream.GetBuffer(), 0, unmanaged, dataLen);

                return unmanaged;
            }
            #endregion

            #region Decode methods
            public void HandleCoderNotFound(int sessionId, int opcode, IntPtr data, int dataLen, int status)
            {
                MemoryStream dataStream = new MemoryStream(dataLen);
                if (dataLen > 0)
                    Marshal.Copy(data, dataStream.GetBuffer(), 0, dataLen);

                Packet packet = new Packet(_svc, sessionId, opcode, dataStream, dataLen, status, data.ToInt64());
                _PushDecodedPacket(sessionId, packet);
            }

            /// <summary>
            /// Decode native packet data method.
            /// <para>if enabled full stack, this method called by LLBC poller threads, otherwise call by Service logic thread</para>
            /// <returns>return Zero pointer if success, otherwise return decode error message(alloc from unmanaged memory area)</returns>
            /// </summary>
            public IntPtr DecodeNative(int sessionId, int opcode, IntPtr data, int dataLen, int status, IntPtr errMsgLen)
            {
                // Copy unmanaged packet data to managed stream.
                MemoryStream dataStream;
                if (dataLen > 0)
                {
                    byte[] bytes = new byte[dataLen];
                    dataStream = new MemoryStream(bytes, 0, dataLen, true, true);
                    Marshal.Copy(data, dataStream.GetBuffer(), 0, dataLen);
                }
                else
                {
                    dataStream = new MemoryStream(0);
                }

                // Get coder, for optimize performance, do minimize lock(just lock coder access code).
                _CoderInfo coderInfo;
                if (_fullStack)
                {
                    lock (_svc._lock)
                    {
                        _svc._coders2.TryGetValue(opcode, out coderInfo);
                    }
                }
                else
                {
                    _svc._coders2.TryGetValue(opcode, out coderInfo);
                }

                // If coder not found, report error to native library.
                // In csharp layer, if DecodeNative() method called, the csharp layer coder must exist.
                // So if not exist coder, return error.
                if (coderInfo == null)
                    return LibUtil.CreateNativeStr(string.Format(
                        "llbc library(c# wrap) internal error: Could not find opcode[{0}]'s coder to decode packet in service '{1}'!", opcode, _svc.svcName), errMsgLen);

                // Decode packet data.
                // If is implemented ICoder coder class, Call ICoder.Decode() to decode packet.
                Packet packet = new Packet(_svc, sessionId, opcode, dataStream, dataLen, status, data.ToInt64());
                if (coderInfo.isICoder)
                {
                    try
                    {
                        ICoder coder = Activator.CreateInstance(coderInfo.coder) as ICoder;
                        if (coder.Decode(dataStream))
                        {
                            packet.data = coder;
                            packet.stream.Position = 0;
                            _PushDecodedPacket(sessionId, packet);

                            return IntPtr.Zero;
                        }
                    }
                    catch (Exception e)
                    {
                        return LibUtil.CreateNativeStr(
                            string.Format("Decode failed, sessionId: {0}, opcode: {1}, dataLen: {2}, exception:\n{3}",
                            sessionId, opcode, dataLen, e), errMsgLen);
                    }

                    // Decode failed.
                    return LibUtil.CreateNativeStr(string.Format(
                        "call c# decode method[{0}] to decode packet[{1}] failed", coderInfo.coder.Name, opcode), errMsgLen);
                }

                // If not found global coder, report error to native library.
                // In csharp layer, if DecodeNative() method called, the csharp layer coder must exist.
                // So if not exist coder, return error.
                if (_svc._globalCoder == null)
                    return LibUtil.CreateNativeStr(string.Format(
                        "llbc library(c# wrap) internal error: Could not find opcode[{0}]'s coder to decode packet in service '{1}'!", opcode, _svc.svcName), errMsgLen);

                // Finalize, use global coder to decode packet.
                try
                {
                    if ((packet.data = _svc._globalCoder.Decode(dataStream, coderInfo.coder)) != null)
                    {
                        packet.stream.Position = 0;
                        _PushDecodedPacket(sessionId, packet);

                        return IntPtr.Zero;
                    }
                }
                catch (Exception e)
                {
                    return LibUtil.CreateNativeStr(
                        string.Format("Decode failed, sessionId: {0}, opcode: {1}, dataLen: {2}, exception:\n{3}",
                        sessionId, opcode, dataLen, e), errMsgLen);
                }

                // Decode failed.
                return LibUtil.CreateNativeStr(string.Format(
                    "call c# decode method[{0}] to decode packet[{1}] failed", coderInfo.coder.Name, opcode), errMsgLen);
            }
            #endregion

            #region Handle/PreHandle/Unify-PreHandle about methods
            /// <summary>
            /// Handle decoded packet.
            /// </summary>
            public void HandlePacket(int sessionId, int opcode, IntPtr data)
            {
                Packet packet = 
                    _GetDecodedPacket(sessionId, data.ToInt64(), true);

                PacketHandler handler;
                if (!_svc._handlers.TryGetValue(packet.opcode, out handler))
                {
                    SafeConsole.Trace("llbc library(c# wrap) internal error: not found packet[{0}] handler", packet.opcode);
                    return;
                }

                try
                {
                    handler.Invoke(packet);
                }
                catch (Exception e)
                {
                    _ProcessPacketHandleException(packet, PacketHandlePhase.Handle, e);
                }
            }

            /// <summary>
            /// PreHandle decoded packet.
            /// </summary>
            public int PreHandlePacket(int sessionId, int opcode, IntPtr data)
            {
                long nativeDataPtr = data.ToInt64();
                Packet packet = _GetDecodedPacket(sessionId, nativeDataPtr, false);

                PacketPreHandler preHandler;
                if (!_svc._preHandlers.TryGetValue(packet.opcode, out preHandler))
                { 
                    SafeConsole.Trace("llbc library(c# wrap) internal error: not found packet[{0}] pre-handler", packet.opcode);
                    return 0;
                }

                try
                {
                    bool ret = preHandler.Invoke(packet);
                    if (!ret)
                        _DeleteDecodedPacket(sessionId, nativeDataPtr);

                    return ret ? 1 : 0;

                }
                catch (Exception e)
                {
                    _ProcessPacketHandleException(packet, PacketHandlePhase.PreHandle, e);
                    _DeleteDecodedPacket(sessionId, nativeDataPtr);
                    return 0;
                }
            }

            /// <summary>
            /// Unify PreHandle decoded packet.
            /// </summary>
            public int UnifyPreHandlePacket(int sessionId, int opcode, IntPtr data)
            {
                if (!_enabledUnifyPreSubscribe)
                    return 1;

                if (_svc._unifyPreHandler == null)
                    return 1;

                long nativeDataPtr = data.ToInt64();
                Packet packet = _GetDecodedPacket(sessionId, nativeDataPtr, false);

                try
                {
                    bool ret = _svc._unifyPreHandler.Invoke(packet);
                    if (!ret)
                        _DeleteDecodedPacket(sessionId, nativeDataPtr);

                    return ret ? 1 : 0;
                }
                catch (Exception e)
                {
                    _ProcessPacketHandleException(packet, PacketHandlePhase.UnifyPreHandle, e);
                    _DeleteDecodedPacket(sessionId, nativeDataPtr);
                    return 0;
                }
            }
            #endregion

            #region cache cleanup methods
            /// <summary>
            /// Remove specific session's all queued packets.
            /// </summary>
            /// <param name="sessionId">session Id</param>
            public void RemoveQueuedPackets(int sessionId)
            {
                if (_decodedPackets.ContainsKey(sessionId))
                    _decodedPackets.Remove(sessionId);
            }

            /// <summary>
            /// Remove specific session's all will encode objects.
            /// </summary>
            /// <param name="sessionId"></param>
            public void RemoveWillSendPackets(int sessionId)
            {
                lock (_willEncodeObjs)
                {
                    if (_willEncodeObjs.ContainsKey(sessionId))
                        _willEncodeObjs.Remove(sessionId);
                }
            }

            public void CleanupAllBufferedDatas()
            {
                lock (_svc._lock)
                {
                    _decodedPackets.Clear();

                    lock (_willEncodeObjs)
                    {
                        _willEncodeObjs.Clear();
                    }
                }
            }
            #endregion

            #region Internal methods
            #region Decoded packets operation support methods
            private void _PushDecodedPacket(int sessionId, Packet packet)
            {
                if (_fullStack)
                {
                    lock (_decodedPackets)
                    {
                        _PushDecodedPacketNonLock(sessionId, packet);
                    }
                }
                else
                {
                    _PushDecodedPacketNonLock(sessionId, packet);
                }
            }

            private void _PushDecodedPacketNonLock(int sessionId, Packet packet)
            {
                List<Packet> packets;
                if (!_decodedPackets.TryGetValue(sessionId, out packets))
                {
                    packets = new List<Packet>();
                    _decodedPackets.Add(sessionId, packets);
                }

                packets.Add(packet);
            }

            private Packet _GetDecodedPacket(int sessionId, long nativeDataPtr, bool dequeue)
            {
                if (_fullStack)
                {
                    lock (_decodedPackets)
                    {
                        return _GetDecodedPacketNonLock(sessionId, nativeDataPtr, dequeue);
                    }
                }
                else
                {
                    return _GetDecodedPacketNonLock(sessionId, nativeDataPtr, dequeue);
                }
            }

            private Packet _GetDecodedPacketNonLock(int sessionId, long nativeDataPtr, bool dequeue)
            {
                List<Packet> packets;
                if (!_decodedPackets.TryGetValue(sessionId, out packets))
                    return null;

                int packetCount = packets.Count;
                for (int i = 0; i < packetCount; ++i)
                {
                    Packet packet = packets[i];
                    if (packet.nativeDataPtr == nativeDataPtr)
                    {
                        if (dequeue)
                            packets.RemoveAt(i);

                        return packet;
                    }
                }

                return null;
            }

            private void _DeleteDecodedPacket(int sessionId, long nativeDataPtr)
            {
                if (_fullStack)
                {
                    lock (_decodedPackets)
                    {
                        _DeleteDecodedPacketNonLock(sessionId, nativeDataPtr);
                    }
                }
                else
                {
                    _DeleteDecodedPacketNonLock(sessionId, nativeDataPtr);
                }
            }

            private void _DeleteDecodedPacketNonLock(int sessionId, long nativeDataPtr)
            {
                List<Packet> packets;
                if (!_decodedPackets.TryGetValue(sessionId, out packets))
                    return;

                int packetCount = packets.Count;
                for (int i = 0; i < packetCount; ++i)
                {
                    if (packets[i].nativeDataPtr == nativeDataPtr)
                    {
                        packets.RemoveAt(i);
                        return;
                    }
                }
            }
            #endregion

            #region Handle/PreHandle/UnifyPreHandle invoke exception handle method
            private void _ProcessPacketHandleException(Packet packet, PacketHandlePhase phase, Exception e)
            {
                PacketExcHandler excHandler;
                if (phase == PacketHandlePhase.PreHandle)
                {
                    if (!_svc._preExcHandlers.TryGetValue(packet.opcode, out excHandler))
                    {
                        if (_svc._dftPreExcHandler != null)
                            excHandler = _svc._dftPreExcHandler;
                    }
                }
                else if (phase == PacketHandlePhase.Handle)
                {
                    if (!_svc._excHandlers.TryGetValue(packet.opcode, out excHandler))
                    {
                        if (_svc._dftExcHandler != null)
                            excHandler = _svc._dftExcHandler;
                    }
                }
                else
                {
                    excHandler = _svc._unifyExcHandler;
                }

                if (excHandler != null)
                {
                    PacketExceptionInfo excInfo = new PacketExceptionInfo();
                    excInfo.packet = packet;
                    excInfo.exception = e;
                    excInfo.phase = phase;

                    try
                    {
                        excHandler(excInfo);
                    }
                    catch (Exception e2)
                    {
                        _svc.RemoveSession(packet.sessionId, string.Format(
                            "Call packet exception handler failed, opcode: {0}, phase: {1}, exception: {2}", packet.opcode, phase, e2));
                        _svc._HandleFrameExc(e2);
                    }
                }
                else
                {
                    _svc.RemoveSession(packet.sessionId,
                        string.Format("Handle packet failed, opcode: {0}, phase: {1}, exception: {2}", packet.opcode, phase, e));
                    _svc._HandleFrameExc(e);
                }
            }
            #endregion
            #endregion

            #region InlComponent data members
            private Service _svc;
            private bool _fullStack;

            private List<IComponent> _comps = new List<IComponent>();
            private List<IComponent> _overridedOnIdleComps = new List<IComponent>();
            private List<IComponent> _overridedOnUpdateComps = new List<IComponent>();

            private Dictionary<int, List<Packet>> _decodedPackets = new Dictionary<int, List<Packet>>();

            private class _WillEncodeObjInfo
            {
                public object obj;
                public bool isCoder;
                public int opcode;
                public int status;
            }
            private Dictionary<int, Dictionary<long, _WillEncodeObjInfo>> _willEncodeObjs = new Dictionary<int, Dictionary<long, _WillEncodeObjInfo>>();
            #endregion
        }
        #endregion

        #region NativeComponent delegates
        private class NativeComponentDelegates
        {
            public NativeComponentDelegates(NativeComponent component)
            {
                svcEncodePacket = component.EncodeManagedObj;
                svcDecodePacket = component.DecodeNative;
                svcPacketHandler = component.HandlePacket;
                svcPacketPreHandler = component.PreHandlePacket;
                svcPacketUnifyPreHandler = component.UnifyPreHandlePacket;
                svcNativeCouldNotFoundDecoderReport = component.HandleCoderNotFound;

                onInit = component.OnInit;
                onDestroy = component.OnDestroy;
                onStart = component.OnStart;
                onStop = component.OnStop;

                onUpdate = component.OnUpdate;
                onIdle = component.OnIdle;

                onSessionCreate = component.OnSessionCreate;
                onSessionDestroy = component.OnSessionDestroy;
                onAsyncConnResult = component.OnAsyncConnResult;

                onProtoReport = component.OnProtoReport;
                onUnHandledPacket = component.OnUnHandledPacket;
            }

            public LLBCNative.Deleg_Service_EncodePacket svcEncodePacket;
            public LLBCNative.Deleg_Service_DecodePacket svcDecodePacket;
            public LLBCNative.Deleg_Service_PacketHandler svcPacketHandler;
            public LLBCNative.Deleg_Service_PacketPreHandler svcPacketPreHandler;
            public LLBCNative.Deleg_Service_PacketUnifyPreHandler svcPacketUnifyPreHandler;
            public LLBCNative.Deleg_Service_NativeCouldNotFoundDecoderReport svcNativeCouldNotFoundDecoderReport;

            public LLBCNative.Deleg_Comp_OnInit onInit;
            public LLBCNative.Deleg_Comp_OnDestroy onDestroy;
            public LLBCNative.Deleg_Comp_OnStart onStart;
            public LLBCNative.Deleg_Comp_OnStop onStop;

            public LLBCNative.Deleg_Comp_OnUpdate onUpdate;
            public LLBCNative.Deleg_Comp_OnIdle onIdle;

            public LLBCNative.Deleg_Comp_OnSessionCreate onSessionCreate;
            public LLBCNative.Deleg_Comp_OnSessionDestroy onSessionDestroy;
            public LLBCNative.Deleg_Comp_OnAsyncConnResult onAsyncConnResult;

            public LLBCNative.Deleg_Comp_OnProtoReport onProtoReport;
            public LLBCNative.Deleg_Comp_OnUnHandledPacket onUnHandledPacket;
        }
        #endregion

        private object _lock = new object();

        // Main data members.
        private string _svcName;
        private ServiceType _svcType;
        private bool _fullStack;
        private IntPtr _llbcSvc = IntPtr.Zero;

        // Components about data members.
        private NativeComponent _nativeComp;
        private NativeComponentDelegates _nativeCompDelegates;

        // Coder about data members.
        IGlobalCoder _globalCoder;
        private class _CoderInfo { public int opcode; public Type coder; public bool isICoder; };
        private Dictionary<Type, _CoderInfo> _coders = new Dictionary<Type, _CoderInfo>();
        private Dictionary<int, _CoderInfo> _coders2 = new Dictionary<int, _CoderInfo>();

        // Packet-Handler about data members.
        private PacketUnifyPreHandler _unifyPreHandler;
        private Dictionary<int, PacketHandler> _handlers = new Dictionary<int, PacketHandler>();
        private Dictionary<int, PacketPreHandler> _preHandlers = new Dictionary<int, PacketPreHandler>();

        // Packet Exception-Handler about data members.
        private PacketExcHandler _dftExcHandler;
        private PacketExcHandler _dftPreExcHandler;
        private PacketExcHandler _unifyExcHandler;
        private Dictionary<int, PacketExcHandler> _excHandlers = new Dictionary<int, PacketExcHandler>();
        private Dictionary<int, PacketExcHandler> _preExcHandlers = new Dictionary<int, PacketExcHandler>();

        // Service frame exception handler.
        private FrameExceptionHandler _frameExcHandler;

        // Disposed flag.
        private bool _disposed;

        // Max packet Id, only available on enabled full-stack support.
        private long _maxPacketId = 1;
        // Static data member, indicate llbc core library enabled unify pre-subscribe support or not in phases of compiler.
        private static bool _enabledUnifyPreSubscribe = LibConfig.commIsEnabledUnifyPreSubscribe;

        private static object _gblLock = new object();
        // Service Id -> Services dict, use to store all created services.
        private static Dictionary<int, Service> _svcId2Svcs = new Dictionary<int, Service>();
        // Service name -> Services dict, use to store all created services.
        private static Dictionary<string, Service> _svcName2Svcs = new Dictionary<string, Service>();
    }
    #endregion
}