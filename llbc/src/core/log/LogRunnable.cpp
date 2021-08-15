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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/objectpool/PoolObjectReflection.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/ILogAppender.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_LogRunnable::LLBC_LogRunnable()
: _stoped(false)
{
}

LLBC_LogRunnable::~LLBC_LogRunnable()
{
}

void LLBC_LogRunnable::Cleanup()
{
    // Output all queued log messages.
    LLBC_Logger *logger = NULL;
    LLBC_MessageBlock *block = NULL;
    while (TryPop(block) == LLBC_OK)
    {
        block->Read(&logger, sizeof(LLBC_LogData *));
        logger->Flush();

        LLBC_Recycle(block);
    }
}

void LLBC_LogRunnable::Svc()
{
    LLBC_Logger *logger = NULL;
    LLBC_MessageBlock *block = NULL;
    while (LIKELY(!_stoped))
    {
        // Try pop log message to output.
        if (TimedPop(block, 50) != LLBC_OK)
            continue;

        block->Read(&logger, sizeof(LLBC_Logger *));

        logger->Flush();

        LLBC_Sleep(100);

        block->SetReadPos(0);
        Push(block);
    }
}

void LLBC_LogRunnable::Stop()
{
    _stoped = true;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
