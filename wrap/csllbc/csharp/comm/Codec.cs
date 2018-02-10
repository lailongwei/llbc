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
    /// Library encodable/decodable object interface encapsulation.
    /// </summary>
    public interface ICoder
    {
        /// <summary>
        /// Encode object to stream.
        /// </summary>
        /// <param name="stream">stream object</param>
        /// <returns>return true if encode success, otherwise return false</returns>
        bool Encode(MemoryStream stream);

        /// <summary>
        /// Decode object from stream.
        /// </summary>
        /// <param name="stream">stream object</param>
        /// <returns>return true if encode success, otherwise return false</returns>
        bool Decode(MemoryStream stream);
    }

    /// <summary>
    /// Library global coder interface encapsulation.
    /// </summary>
    public interface IGlobalCoder
    {
        /// <summary>
        /// Encode object to stream.
        /// </summary>
        /// <param name="stream">stream</param>
        /// <param name="obj">object</param>
        /// <returns>return true if encode success, otherwise return false</returns>
        bool Encode(MemoryStream stream, object obj);

        /// <summary>
        /// Decode specific type object from stream.
        /// </summary>
        /// <param name="stream">stream</param>
        /// <param name="cls">object type</param>
        /// <returns>decoded object, if decode failed</returns>
        object Decode(MemoryStream stream, Type cls);
    }
}