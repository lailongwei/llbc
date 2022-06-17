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

#ifndef __LLBC_COMM_COMPONENT_EVENTS_H__
#define __LLBC_COMM_COMPONENT_EVENTS_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The component events offset enumeration.
 */
class LLBC_ComponentEventsOffset
{
public:
    enum
    {
        Begin = 0,

        OnInitialize = Begin,
        OnDestroy,

        OnStart,
        OnStop,

        OnUpdate,
        OnIdle,

        OnSessionCreate,
        OnSessionDestroy,
        OnAsyncConnResult,
        OnProtoReport,
        OnUnHandledPacket,

        OnAppCfgReloaded,

        End,
    };
};

/**
 * \brief The component events id define.
 */
class LLBC_ComponentEvents
{
public:
    static constexpr uint64 OnInitialize = 1 << LLBC_ComponentEventsOffset::OnInitialize;
    static constexpr uint64 OnDestroy = 1 << LLBC_ComponentEventsOffset::OnDestroy;
    
    static constexpr uint64 OnStart = 1 << LLBC_ComponentEventsOffset::OnStart;
    static constexpr uint64 OnStop = 1 << LLBC_ComponentEventsOffset::OnStop;

    static constexpr uint64 OnUpdate = 1 << LLBC_ComponentEventsOffset::OnUpdate;
    static constexpr uint64 OnIdle = 1 << LLBC_ComponentEventsOffset::OnIdle;

    static constexpr uint64 OnSessionCreate = 1 << LLBC_ComponentEventsOffset::OnSessionCreate;
    static constexpr uint64 OnSessionDestroy = 1 << LLBC_ComponentEventsOffset::OnSessionDestroy;
    static constexpr uint64 OnAsyncConnResult = 1 << LLBC_ComponentEventsOffset::OnAsyncConnResult;
    static constexpr uint64 OnProtoReport = 1 << LLBC_ComponentEventsOffset::OnProtoReport;
    static constexpr uint64 OnUnHandledPacket = 1 << LLBC_ComponentEventsOffset::OnUnHandledPacket;
    static constexpr uint64 OnAppCfgReloaded = 1 << LLBC_ComponentEventsOffset::OnAppCfgReloaded;

public:
    static constexpr uint64 AllEvents = LLBC_ComponentEvents::OnInitialize | LLBC_ComponentEvents::OnDestroy |
                                        LLBC_ComponentEvents::OnStart | LLBC_ComponentEvents::OnStop |
                                        LLBC_ComponentEvents::OnUpdate | LLBC_ComponentEvents::OnIdle |
                                        LLBC_ComponentEvents::OnSessionCreate | LLBC_ComponentEvents::OnSessionDestroy |
                                        LLBC_ComponentEvents::OnAsyncConnResult | LLBC_ComponentEvents::OnProtoReport |
                                        LLBC_ComponentEvents::OnUnHandledPacket |
                                        LLBC_ComponentEvents::OnAppCfgReloaded;

    static constexpr uint64 DefaultEvents = LLBC_ComponentEvents::AllEvents & ~(LLBC_ComponentEvents::OnUpdate | LLBC_ComponentEvents::OnIdle);
};

__LLBC_NS_END

#endif // !__LLBC_COMM_COMPONENT_EVENTS_H__
