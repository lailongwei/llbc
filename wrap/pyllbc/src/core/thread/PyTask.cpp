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


#include "pyllbc/common/Export.h"

#include "pyllbc/core/thread/PyTask.h"
#include "pyllbc/core/thread/PyTaskMgr.h"

pyllbc_Task::pyllbc_Task(int taskId, const LLBC_String &script)
{
    _taskId = taskId;
    _script.append(script);

    _activated = false;
}

pyllbc_Task::~pyllbc_Task()
{
}

int pyllbc_Task::GetId() const
{
    return _taskId;
}

bool pyllbc_Task::IsActivated() const
{
    return _activated;
}

void pyllbc_Task::Svc()
{
    Py_Initialize();
    PyRun_SimpleString(_script.c_str());

    _activated = true;
    pyllbc_s_TaskMgr->OnTaskCreated(this);
}

void pyllbc_Task::Cleanup()
{
    pyllbc_s_TaskMgr->OnTaskDestroy(this);
    _activated = false;

    Py_Finalize();
}

