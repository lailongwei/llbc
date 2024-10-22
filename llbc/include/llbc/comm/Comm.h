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

#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/Packet.h"
#include "llbc/comm/Coder.h"
#include "llbc/comm/Component.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/Service.h"
#include "llbc/comm/ServiceMgr.h"
#include "llbc/comm/ServiceEventFirer.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"
#include "llbc/comm/protocol/RawProtocolFactory.h"
#include "llbc/comm/protocol/NormalProtocolFactory.h"
#include "llbc/comm/protocol/RawProtocol.h"
#include "llbc/comm/protocol/PacketProtocol.h"
#include "llbc/comm/protocol/CompressProtocol.h"
#include "llbc/comm/protocol/CodecProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"
#include "llbc/comm/protocol/IProtocolFilter.h"

__LLBC_NS_BEGIN

/**
 * LLBC library Communication-Module startup function, before use any
 * this module's functions or classes, must call this function to initialize
 * the module.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_HIDDEN int __LLBC_CommStartup();

/**
 * LLBC library Communication-Module cleanup function.
 */
LLBC_HIDDEN void __LLBC_CommCleanup();

__LLBC_NS_END
