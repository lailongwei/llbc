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


#include "llbc/common/Export.h"

#include "llbc/comm/protocol/ProtocolLayer.h"

namespace
{
    typedef LLBC_NS LLBC_ProtocolLayer This;
}

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __g_descs[] =
{
    "PackLayer",
    "CompressLayer",
    "CodecLayer",

    "Invalid"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

bool LLBC_ProtocolLayer::IsValid(int layer)
{
    return (This::Begin <= layer && layer < This::End);
}

const LLBC_String &LLBC_ProtocolLayer::Layer2Str(int layer)
{
    return LLBC_INL_NS __g_descs[This::IsValid(layer) ? layer : This::End];
}

__LLBC_NS_END
