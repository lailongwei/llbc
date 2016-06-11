/**
 * @file    Codec.cs
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