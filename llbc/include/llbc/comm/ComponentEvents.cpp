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
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/ComponentEvents.h"

__LLBC_NS_BEGIN

const uint64 LLBC_ComponentEvents::OnInitialize = 1 << LLBC_ComponentEventsOffset::OnInitialize;
const uint64 LLBC_ComponentEvents::OnDestroy = 1 << LLBC_ComponentEventsOffset::OnDestroy;

const uint64 LLBC_ComponentEvents::OnStart = 1 << LLBC_ComponentEventsOffset::OnStart;
const uint64 LLBC_ComponentEvents::OnStop = 1 << LLBC_ComponentEventsOffset::OnStop;

const uint64 LLBC_ComponentEvents::OnUpdate = 1 << LLBC_ComponentEventsOffset::OnUpdate;
const uint64 LLBC_ComponentEvents::OnIdle = 1 << LLBC_ComponentEventsOffset:: OnIdle;

const uint64 LLBC_ComponentEvents::OnSessionCreate = 1 << LLBC_ComponentEventsOffset::OnSessionCreate;
const uint64 LLBC_ComponentEvents::OnSessionDestroy = 1 << LLBC_ComponentEventsOffset::OnSessionDestroy;
const uint64 LLBC_ComponentEvents::OnAsyncConnResult = 1 << LLBC_ComponentEventsOffset::OnAsyncConnResult;
const uint64 LLBC_ComponentEvents::OnProtoReport = 1 << LLBC_ComponentEventsOffset::OnProtoReport;
const uint64 LLBC_ComponentEvents::OnUnHandledPacket = 1 << LLBC_ComponentEventsOffset::OnUnHandledPacket;
const uint64 LLBC_ComponentEvents::OnAppCfgReloaded = 1 << LLBC_ComponentEventsOffset::OnAppCfgReloaded;

const uint64 LLBC_ComponentEvents::AllEvents = LLBC_ComponentEvents::OnInitialize | LLBC_ComponentEvents::OnDestroy |
                                               LLBC_ComponentEvents::OnStart | LLBC_ComponentEvents::OnStop |
                                               LLBC_ComponentEvents::OnUpdate | LLBC_ComponentEvents::OnIdle |
                                               LLBC_ComponentEvents::OnSessionCreate | LLBC_ComponentEvents::OnSessionDestroy |
                                               LLBC_ComponentEvents::OnAsyncConnResult | LLBC_ComponentEvents::OnProtoReport |
                                               LLBC_ComponentEvents::OnUnHandledPacket |
                                               LLBC_ComponentEvents::OnAppCfgReloaded;

const uint64 LLBC_ComponentEvents::DefaultEvents = LLBC_ComponentEvents::AllEvents & ~(LLBC_ComponentEvents::OnUpdate | LLBC_ComponentEvents::OnIdle);

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
