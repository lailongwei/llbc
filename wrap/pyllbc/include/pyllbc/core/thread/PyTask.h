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

#ifndef __PYLLBC_CORE_THREAD_PY_TASK_H__
#define __PYLLBC_CORE_THREAD_PY_TASK_H__

#include "pyllbc/common/Common.h"

/**
 * \brief The pyllbc library task class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Task : public LLBC_BaseTask
{
public:
    pyllbc_Task(int taskId, const LLBC_String &script);
    virtual ~pyllbc_Task();

public:
    /**
     * Get the task Id.
     * @return int - the task Id.
     */
    int GetId() const;

    /**
     * Return task activated attribute.
     * @return bool - return true if activated, otherwise return false.
     */
    bool IsActivated() const;

public:
    /**
     * Task service method.
     */
    virtual void Svc();

    /**
     * Task cleanup method.
     */
    virtual void Cleanup();

private:
    int _taskId;
    LLBC_String _script;

    volatile bool _activated;
};

#endif // !__PYLLBC_CORE_THREAD_PY_TASK_H__
