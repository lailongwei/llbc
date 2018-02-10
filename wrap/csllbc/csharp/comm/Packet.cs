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