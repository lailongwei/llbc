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

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The component events offset enumeration.
 */
class LLBC_ComponentEventIndex
{
public:
    enum
    {
        Begin = 0,

        OnInit = Begin,
        OnDestroy,

        OnStart,
        OnLateStart,
        OnEarlyStop,
        OnStop,

        OnUpdate,
        OnIdle,

        OnSessionCreate,
        OnSessionDestroy,
        OnAsyncConnResult,
        OnProtoReport,
        OnUnHandledPacket,

        OnApplicationWillStart,
        OnApplicationStartFail,
        OnApplicationStartFinish,
        OnApplicationWillStop,
        // OnApplicationStopFinish, // Note: 
                                    //       When application stop finished, all service(s) and it's component(s) 
                                    //       has been deleted, so this event index will not define.
        OnApplicationConfigReload,

        End,
    };
};

/**
 * \brief The component events id define.
 */
class LLBC_EXPORT LLBC_ComponentEvents
{
public:
    static constexpr uint64 OnInit = 1 << LLBC_ComponentEventIndex::OnInit;
    static constexpr uint64 OnDestroy = 1 << LLBC_ComponentEventIndex::OnDestroy;
    
    static constexpr uint64 OnStart = 1 << LLBC_ComponentEventIndex::OnStart;
    static constexpr uint64 OnLateStart = 1 << LLBC_ComponentEventIndex::OnLateStart;
    static constexpr uint64 OnEarlyStop = 1 << LLBC_ComponentEventIndex::OnEarlyStop;
    static constexpr uint64 OnStop = 1 << LLBC_ComponentEventIndex::OnStop;

    static constexpr uint64 OnUpdate = 1 << LLBC_ComponentEventIndex::OnUpdate;
    static constexpr uint64 OnIdle = 1 << LLBC_ComponentEventIndex::OnIdle;

    static constexpr uint64 OnSessionCreate = 1 << LLBC_ComponentEventIndex::OnSessionCreate;
    static constexpr uint64 OnSessionDestroy = 1 << LLBC_ComponentEventIndex::OnSessionDestroy;
    static constexpr uint64 OnAsyncConnResult = 1 << LLBC_ComponentEventIndex::OnAsyncConnResult;
    static constexpr uint64 OnProtoReport = 1 << LLBC_ComponentEventIndex::OnProtoReport;
    static constexpr uint64 OnUnHandledPacket = 1 << LLBC_ComponentEventIndex::OnUnHandledPacket;

    static constexpr uint64 OnApplicationWillStart = 1 << LLBC_ComponentEventIndex::OnApplicationWillStart;
    static constexpr uint64 OnApplicationStartFail = 1 << LLBC_ComponentEventIndex::OnApplicationStartFail;
    static constexpr uint64 OnApplicationStartFinish = 1 << LLBC_ComponentEventIndex::OnApplicationStartFinish;
    static constexpr uint64 OnApplicationWillStop = 1 << LLBC_ComponentEventIndex::OnApplicationWillStop;
    static constexpr uint64 OnApplicationConfigReload = 1 << LLBC_ComponentEventIndex::OnApplicationConfigReload;

public:
    static constexpr uint64 AllEvents = (1 << LLBC_ComponentEventIndex::End) - 1;
    static constexpr uint64 DefaultEvents = LLBC_ComponentEvents::AllEvents &
                                            ~(LLBC_ComponentEvents::OnUpdate |
                                              LLBC_ComponentEvents::OnIdle);

public:
    /**
     * Get event index.
     * @param[in] ev - the component event.
     * @return int - the event index.
     */
    static int IndexOf(uint64 ev);
};

__LLBC_NS_END
