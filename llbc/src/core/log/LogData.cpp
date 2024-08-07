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

#include "llbc/core/objpool/ObjPool.h"

#include "llbc/core/log/LogData.h"

__LLBC_NS_BEGIN
// Note: Some data members don't need init.
LLBC_LogData::LLBC_LogData()
// : logger(nullptr)

: msg(nullptr)
, msgLen(0)
, msgCap(0)

// , level(-1)
// , logTime(0)

, fileLen(0)
, funcLen(0)
, tagLen(0)

// , line(0)

// , threadId(LLBC_INVALID_NATIVE_THREAD_ID)

, _typedObjPool(nullptr)
{
}

LLBC_LogData::~LLBC_LogData()
{
    if (msg)
        free(msg);
}

void LLBC_LogData::Reuse()
{
    // Notes:
    // - Some data members don't need reset.
    // - Free msg buf, if cap too large.

    // logger = nullptr;

    msgLen = 0;
    if (msgCap >= MAX(1024, (LLBC_CFG_LOG_FORMAT_BUF_SIZE * 7 / 8)))
    {
        free(msg);
        msg = nullptr;
        msgCap = 0;
    }

    // level = -1;
    // logTime = 0;

    fileLen = 0;
    funcLen = 0;
    tagLen = 0;

    // line = 0;

    // threadId = LLBC_INVALID_NATIVE_THREAD_ID;
}

LLBC_TypedObjPool<LLBC_LogData> *LLBC_LogData::GetTypedObjPool() const
{
    return _typedObjPool;
}

void LLBC_LogData::SetTypedObjPool(LLBC_TypedObjPool<LLBC_LogData> *typedObjPool)
{
    _typedObjPool = typedObjPool;
}

__LLBC_NS_END
