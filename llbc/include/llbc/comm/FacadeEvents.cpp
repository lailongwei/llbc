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

#include "llbc/comm/FacadeEvents.h"

__LLBC_NS_BEGIN

const uint64 LLBC_FacadeEvents::OnInitialize = 1 << LLBC_FacadeEventsOffset::OnInitialize;
const uint64 LLBC_FacadeEvents::OnDestroy = 1 << LLBC_FacadeEventsOffset::OnDestroy;

const uint64 LLBC_FacadeEvents::OnStart = 1 << LLBC_FacadeEventsOffset::OnStart;
const uint64 LLBC_FacadeEvents::OnStop = 1 << LLBC_FacadeEventsOffset::OnStop;

const uint64 LLBC_FacadeEvents::OnUpdate = 1 << LLBC_FacadeEventsOffset::OnUpdate;
const uint64 LLBC_FacadeEvents::OnIdle = 1 << LLBC_FacadeEventsOffset:: OnIdle;

const uint64 LLBC_FacadeEvents::OnSessionCreate = 1 << LLBC_FacadeEventsOffset::OnSessionCreate;
const uint64 LLBC_FacadeEvents::OnSessionDestroy = 1 << LLBC_FacadeEventsOffset::OnSessionDestroy;
const uint64 LLBC_FacadeEvents::OnAsyncConnResult = 1 << LLBC_FacadeEventsOffset::OnAsyncConnResult;
const uint64 LLBC_FacadeEvents::OnProtoReport = 1 << LLBC_FacadeEventsOffset::OnProtoReport;
const uint64 LLBC_FacadeEvents::OnUnHandledPacket = 1 << LLBC_FacadeEventsOffset::OnUnHandledPacket;
const uint64 LLBC_FacadeEvents::OnAppCfgReloaded = 1 << LLBC_FacadeEventsOffset::OnAppCfgReloaded;

const uint64 LLBC_FacadeEvents::AllEvents = LLBC_FacadeEvents::OnInitialize | LLBC_FacadeEvents::OnDestroy |
                                            LLBC_FacadeEvents::OnStart | LLBC_FacadeEvents::OnStop |
                                            LLBC_FacadeEvents::OnUpdate | LLBC_FacadeEvents::OnIdle |
                                            LLBC_FacadeEvents::OnSessionCreate | LLBC_FacadeEvents::OnSessionDestroy |
                                            LLBC_FacadeEvents::OnAsyncConnResult | LLBC_FacadeEvents::OnProtoReport |
                                            LLBC_FacadeEvents::OnUnHandledPacket |
                                            LLBC_FacadeEvents::OnAppCfgReloaded;

const uint64 LLBC_FacadeEvents::DefaultEvents = LLBC_FacadeEvents::AllEvents & ~(LLBC_FacadeEvents::OnUpdate | LLBC_FacadeEvents::OnIdle);

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
