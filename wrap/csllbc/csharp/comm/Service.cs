/**
 * @file    Service.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 * 
 * @brief
 */

using System;

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
    }
    #endregion

    #region Service
    /// <summary>
    /// The service class encapsulation.
    /// </summary>
    public class Service : IDisposable
    {
        #region Ctor/Dtor
        /// <summary>
        ///  Construct service object with specific ServiceType.
        /// </summary>
        /// <param name="svcType">service type enumeration</param>
        public Service(ServiceType svcType)
        {
            _llbcSvc = LLBCNative.csllbc_Service_Create((int)svcType);
            if (_llbcSvc.ToInt64() == 0)
                throw ExceptionUtil.CreateExceptionFromCoreLib();

            _svcType = svcType;
        }

        /// <summary>
        /// Destruct service object.
        /// </summary>
        ~Service()
        {
            Dispose(false);
        }
        #endregion

        #region svcType, isStarted
        /// <summary>
        /// Get the service type.
        /// </summary>
        public ServiceType svcType
        {
            get { return _svcType; }
        }

        /// <summary>
        /// Check service started or not.
        /// </summary>
        public bool isStarted
        {
            get
            {
                if (_llbcSvc == IntPtr.Zero)
                    return false;

                return LLBCNative.csllbc_Service_IsStarted(_llbcSvc);
            }
        }
        #endregion

        public void Start(int pollerCount)
        {
            if (LLBCNative.csllbc_Service_Start(_llbcSvc, pollerCount) != 0)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }

        public void Stop()
        {
            LLBCNative.csllbc_Service_Stop(_llbcSvc);
        }

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
        virtual protected void Dispose(bool disposing)
        {
            lock (_lock)
            {
                if (_disposed)
                    return;

                LLBCNative.csllbc_Service_Delete(_llbcSvc);
                _llbcSvc = IntPtr.Zero;
                _svcType = ServiceType.Normal;

                if (disposing)
                    GC.SuppressFinalize(this);

                _disposed = true;
            }
        }
        #endregion

        private object _lock = new object();

        private ServiceType _svcType;
        private IntPtr _llbcSvc = IntPtr.Zero;

        private bool _disposed;
    }
    #endregion
}