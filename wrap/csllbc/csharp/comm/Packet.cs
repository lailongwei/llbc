/**
 * @file    Packet.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/23
 * @version 1.0
 * 
 * @brief
 */

using System;
using System.IO;

namespace llbc
{
    /// <summary>
    /// Library packet class encapsulation.
    /// </summary>
    public class Packet
    {
        /// <summary>
        /// Construct packet.
        /// </summary>
        public Packet(Service svc, int sessionId, int opcode, MemoryStream stream, int streamLen, int status, long nativeDataPtr)
        {
            _svc = svc;
            _sessionId = sessionId;
            _opcode = opcode;
            _stream = stream;
            _streamLength = streamLen;
            _status = status;
            _nativeDataPtr = nativeDataPtr;
        }

        /// <summary>
        /// Get create this packet's service object.
        /// </summary>
        public Service svc
        {
            get { return _svc; }
        }

        /// <summary>
        /// Get sessionId.
        /// </summary>
        public int sessionId
        {
            get { return _sessionId; }
        }

        /// <summary>
        /// Get opcode.
        /// </summary>
        public int opcode
        {
            get { return _opcode; }
        }

        /// <summary>
        /// Get status.
        /// </summary>
        public int status
        {
            get { return _status; }
        }

        /// <summary>
        /// Get/Set data.
        /// </summary>
        public object data
        {
            get { return _data; }
            set { _data = value; }
        }

        /// <summary>
        /// Get stream.
        /// </summary>
        public MemoryStream stream
        {
            get { return _stream; }
        }

        /// <summary>
        /// Get stream length.
        /// </summary>
        public int streamLength
        {
            get { return _streamLength; }
        }

        /// <summary>
        /// Get native packet data pointer.
        /// </summary>
        public long nativeDataPtr
        {
            get { return _nativeDataPtr; }
        }

        public override string ToString()
        {
            return string.Format(
                "sessionId:{0},opcode:{1},dataLen:{2},status:{3}:data:{4}", 
                _sessionId, _opcode, _streamLength, _status, _data);
        }

        private Service _svc;
        private int _sessionId;

        private int _opcode;
        private int _status;

        private object _data;
        private MemoryStream _stream;
        private int _streamLength;

        private long _nativeDataPtr;
    }
}