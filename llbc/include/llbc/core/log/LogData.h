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

#ifndef __LLBC_CORE_LOG_LOG_DATA_H__
#define __LLBC_CORE_LOG_LOG_DATA_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_IObjectPoolInst;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log data structure encapsulation.
 */
struct LLBC_EXPORT LLBC_LogData
{
    char *msg;                            // Log message(allocate from heap).
    uint32 msgLen;                        // message length.

    int level;                            // Log level.
    const char *loggerName;               // Logger name.

    char *others;                         // Others data(allocate from heap).
    uint32 othersSize;                    // Others data size.
    uint32 tagBeg;                        // Tag begin.
    uint32 tagLen;                        // Tag length.

    sint64 logTime;                       // Log time.

    uint32 fileBeg;                       // Log source file name begin.
    uint32 fileLen;                       // Log source file name length.
    long line;                            // Log source file line number.

    LLBC_ThreadId threadId;               // Log native thread Id.

public:
    /**
     * Constructor & Destructor.
     */
    LLBC_LogData();
    ~LLBC_LogData();

public:
    /**
     * Object pool support method.
     */
    void Clear();

    /**
     * Object pool support method.
     */
    bool IsPoolObject() const;

    /**
     * Object pool support method.
     */
    void MarkPoolObject(LLBC_IObjectPoolInst &poolInst);

    /**
     * Object pool support method.
     */
    void GiveBackToPool();

    /**
     * Object pool support method.
     */
    LLBC_IObjectPoolInst *GetPoolInst();

private:
    LLBC_IObjectPoolInst *_poolInst;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_DATA_H__
