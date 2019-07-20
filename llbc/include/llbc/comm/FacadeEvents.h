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

#ifndef __LLBC_COMM_FACADE_EVENTS_H__
#define __LLBC_COMM_FACADE_EVENTS_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The facade events offset enumeration.
 */
class LLBC_EXPORT LLBC_FacadeEventsOffset
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

        End,
    };
};

/**
 * \brief The facade events id define.
 */
class LLBC_EXPORT LLBC_FacadeEvents
{
public:
    static const uint64 OnInitialize;
    static const uint64 OnDestroy;
    
    static const uint64 OnStart;
    static const uint64 OnStop;

    static const uint64 OnUpdate;
    static const uint64 OnIdle;

    static const uint64 OnSessionCreate;
    static const uint64 OnSessionDestroy;
    static const uint64 OnAsyncConnResult;
    static const uint64 OnProtoReport;
    static const uint64 OnUnHandledPacket;

public:
    static const uint64 AllEvents;
    static const uint64 DefaultEvents;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_FACADE_EVENTS_H__
