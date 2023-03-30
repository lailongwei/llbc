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

#pragma once

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The protocol layers enumeartion.
 */
class LLBC_EXPORT LLBC_ProtocolLayer
{
public:
    enum
    {
        Begin,

        // Pack/Unpack layer, be responsible for raw bytestream and packet conversion.
        PackLayer = Begin,
        // Compress/DeCompress layer, be responsible for comp/decomp packet.
        CompressLayer,
        // Encode/Decode layer, be responsible for encode/decode packet.
        CodecLayer,

        End
    };

public:
    /**
     * Check given protocol layer legal or not.
     * @param[in] layer - the protocol layer.
     * @return bool - return true if validate, otherwise return -1.
     */
    static bool IsValid(int layer);

    /**
     * Get the protocol layer string representation.
     * @param[in] layer - protocol layer.
     * @return const LLBC_String & - the protocol layer string representation.
     */
    static const LLBC_String &Layer2Str(int layer);
};

__LLBC_NS_END
