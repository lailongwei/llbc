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
#include "llbc/comm/protocol/PacketProtocol.h"
#include "llbc/comm/protocol/CompressProtocol.h"
#include "llbc/comm/protocol/CodecProtocol.h"
#include "llbc/comm/protocol/NormalProtocolFactory.h"

__LLBC_NS_BEGIN

LLBC_IProtocol *LLBC_NormalProtocolFactory::Create(int layer) const
{
    switch (layer)
    {
    case LLBC_ProtocolLayer::CodecLayer:
        return new LLBC_CodecProtocol;

    case LLBC_ProtocolLayer::CompressLayer:
        return new LLBC_CompressProtocol;

    case LLBC_ProtocolLayer::PackLayer:
        return new LLBC_PacketProtocol;

    default:
        return nullptr;
    }
}

__LLBC_NS_END
