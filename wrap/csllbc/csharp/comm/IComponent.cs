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
using System.IO;
using System.Net;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace llbc
{
    #region SessionInfo
    /// <summary>
    /// The session information class encapsulation.
    /// </summary>
    public class SessionInfo
    {
        public SessionInfo(bool isListen,
                           int sessionId,
                           int acceptSessionId,
                           int socketHandle,
                           IPEndPoint localEndPoint,
                           IPEndPoint remoteEndPoint)
        {
            _isListen = isListen;

            _sessionId = sessionId;
            _acceptSessionId = acceptSessionId;
            _socketHandle = socketHandle;

            _localEndPoint = localEndPoint;
            _remoteEndPoint = remoteEndPoint;

            _BuildStringRepr();
        }

        public bool isListenSession
        {
            get { return _isListen; }
        }
        public int sessionId
        {
            get { return _sessionId; }
        }

        public int acceptSessionId
        {
            get { return _acceptSessionId; }
        }
        public int socketHandle
        {
            get { return _socketHandle; }
        }

        public IPEndPoint localEndPoint
        {
            get { return _localEndPoint; }
        }

        public string localHost
        {
            get { return _localEndPoint.Address.ToString(); }
        }

        public int localPort
        {
            get { return _localEndPoint.Port; }
        }

        public IPEndPoint remoteEndPoint
        {
            get { return _remoteEndPoint; }
        }

        public string remoteHost
        {
            get { return _remoteEndPoint.Address.ToString(); }
        }

        public int remotePort
        {
            get { return _remoteEndPoint.Port; }
        }

        public override string ToString()
        {
            return _repr;
        }

        #region Internal implementaions
        private void _BuildStringRepr()
        {
            _repr = string.Format(
                "SessionInfo: [sessionId: {0}, acceptSessionId: {1}, socketHandle: {2}, isListen: {3}, localEP: {4}, remoteEP: {5}]",
                _sessionId, _acceptSessionId, _socketHandle, _isListen, _localEndPoint, _remoteEndPoint);
        }
        #endregion

        bool _isListen;

        int _sessionId;
        int _acceptSessionId;
        int _socketHandle;

        IPEndPoint _localEndPoint;
        IPEndPoint _remoteEndPoint;

        string _repr;
    }
    #endregion

    #region SessionDestroyInfo
    /// <summary>
    /// The session destroy information class encapsulation.
    /// </summary>
    public class SessionDestroyInfo
    {
        public SessionDestroyInfo(SessionInfo sessionInfo,
                                  bool fromSvc,
                                  string reason,
                                  int errNo,
                                  int subErrNo)
        {
            _sessionInfo = sessionInfo;

            _fromSvc = fromSvc;
            _reason = reason;

            _errNo = errNo;
            _subErrNo = subErrNo;

            _BuildStringRepr();
        }

        public SessionInfo sessionInfo
        {
            get { return _sessionInfo; }
        }

        public int sessionId
        {
            get { return _sessionInfo.sessionId; }
        }

        public bool isDestroyedFromService
        {
            get { return _fromSvc; }
        }

        public string reason
        {
            get { return _reason; }
        }

        public int errNo
        {
            get { return _errNo; }
        }

        public int subErrNo
        {
            get { return _subErrNo; }
        }

        public override string ToString()
        {
            return _repr;
        }

        private void _BuildStringRepr()
        {
            _repr = string.Format(
                "SessionDestroyInfo: [sessionInfo: {0}, fromSvc: {1}, reason: {2}]", _sessionInfo, _fromSvc, _reason);
        }

        private SessionInfo _sessionInfo;

        private bool _fromSvc;
        private string _reason;

        private int _errNo;
        private int _subErrNo;

        private string _repr;
    }
    #endregion

    #region AsyncConnResult
    /// <summary>
    /// The Service.AsyncConn result information class encapsulation.
    /// </summary>
    public class AsyncConnResult
    {
        public AsyncConnResult(bool connected, string reason, IPEndPoint remoteEndPoint)
        {
            _connected = connected;
            _reason = reason;
            _remoteEndPoint = remoteEndPoint;

            _BuildStringRepr();
        }

        public bool connected
        {
            get { return _connected; }
        }

        public string reason
        {
            get { return _reason; }
        }

        public IPEndPoint remoteEndPoint
        {
            get { return _remoteEndPoint; }
        }

        public string remoteHost
        {
            get { return _remoteEndPoint.Address.ToString(); }
        }

        public int remotePort
        {
            get { return _remoteEndPoint.Port; }
        }

        public override string ToString()
        {
            return _repr;
        }

        private void _BuildStringRepr()
        {
            _repr = string.Format(
                "AsyncConnResult: [connected: {0}, reason: {1}, remoteEndPoint: {2}]", _connected, _reason, _remoteEndPoint);
        }

        private bool _connected;
        private string _reason;
        private IPEndPoint _remoteEndPoint;

        private string _repr;
    }
    #endregion

    #region ProtoReport
    public class ProtoReport
    {
        public ProtoReport(int sessionId,
                           ProtoLayer layer,
                           ProtoReportLevel level,
                           string report)
        {
            _sessionId = sessionId;
            _layer = layer;
            _reportLevel = level;
            _report = report;

            _BuildStringRepr();   
        }

        public int sessionId
        {
            get { return _sessionId; }
        }

        public ProtoLayer layer
        {
            get { return _layer; }
        }

        public ProtoReportLevel reportLevel
        {
            get { return _reportLevel; }
        }

        public string report
        {
            get { return _report; }
        }

        public override string ToString()
        {
            return _repr;
        }

        private void _BuildStringRepr()
        {
            _repr = string.Format(
                "ProtoReport: [sessionId: {0}, layer: {1}, level: {2}, report: {3}]",
                _sessionId, _layer, _reportLevel, _report);
        }

        private int _sessionId;
        private ProtoLayer _layer;
        private ProtoReportLevel _reportLevel;
        private string _report;

        private string _repr;
    }
    #endregion

    /// <summary>
    /// Service component base class encapsulation.
    /// </summary>
    public class IComponent
    {
        /// <summary>
        /// Service getter/setter.
        /// </summary>
        public Service svc
        {
            get { return _svc; }
            set { _svc = value; }
        }

        #region OnInit/OnDestroy, OnStart/OnStop
        /// <summary>
        /// When component first start, will call this method to initialize component before OnStart method call.
        /// </summary>
        public virtual void OnInit() { }

        /// <summary>
        /// When service destroy, will call this method to destroy component.
        /// </summary>
        public virtual void OnDestroy() { }

        /// <summary>
        /// When service startup, will call this method.
        /// </summary>
        public virtual void OnStart() { }

        /// <summary>
        /// When service stop, will call this method.
        /// </summary>
        public virtual void OnStop() { }
        #endregion

        #region OnUpdate/OnIdle
        /// <summary>
        /// Service update handler.
        /// </summary>
        public virtual void OnUpdate() { }

        /// <summary>
        /// Service per-frame idle handler.
        /// </summary>
        /// <param name="idleTime">idle time, in micro-seconds</param>
        public virtual void OnIdle(int idleTime) { }
        #endregion

        #region SessionEvents: OnSessionCreate/OnSessionDestroy/OnAsyncConnResult
        /// <summary>
        /// When new session create, will call this handler to process.
        /// </summary>
        /// <param name="sessionInfo">new session info</param>
        public virtual void OnSessionCreate(SessionInfo sessionInfo) { }

        /// <summary>
        /// When session destroyed, will call this handler to process.
        /// </summary>
        /// <param name="destroyInfo">destroy info</param>
        public virtual void OnSessionDestroy(SessionDestroyInfo destroyInfo) { }

        /// <summary>
        /// When a Service.AsyncConn result, will call this handler to process.
        /// </summary>
        /// <param name="asyncConnResult">async-connect result info</param>
        public virtual void OnAsyncConnResult(AsyncConnResult asyncConnResult) { }
        #endregion

        #region OnProtoReport/OnUnHandledPacket
        /// <summary>
        /// When proto-stack has message to report, will call this handler to process.
        /// </summary>
        /// <param name="report">report message info</param>
        public virtual void OnProtoReport(ProtoReport report) { }

        /// <summary>
        /// When one packet unhandled, will call this handler to process.
        /// </summary>
        /// <param name="opcode">unhandled packet opcode</param>
        public virtual void OnUnHandledPacket(Packet packet) { }
        #endregion

        private Service _svc;
    }
}
