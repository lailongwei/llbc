/**
 * @file    PyTaskMgr.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/core/thread/PyTask.h"
#include "pyllbc/core/thread/PyTaskMgr.h"

namespace
{
    typedef pyllbc_TaskMgr This;
}

pyllbc_TaskMgr::pyllbc_TaskMgr()
: _lock()

, _tasks()
, _maxTaskId(0)
{
}

pyllbc_TaskMgr::~pyllbc_TaskMgr()
{
    LLBC_Guard guard(_lock);
    
    while(!_tasks.empty())
        LLBC_ThreadManager::Sleep(20);
}

int pyllbc_TaskMgr::CreateTask(const LLBC_String &script)
{
    if (script.empty())
    {
        pyllbc_SetError("task entry script empty", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);

    if (GetTaskCount() >= PYLLBC_CFG_TASK_LMT)
    {
        pyllbc_SetError("task count gone to limit, create failed", LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    pyllbc_Task *task = new pyllbc_Task(++_maxTaskId, script);
    _tasks.insert(std::make_pair(_maxTaskId, task));

    if (task->Activate() != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);

        _tasks.erase(_maxTaskId--);
        delete task;

        return LLBC_FAILED;
    }

    return _maxTaskId;
}

pyllbc_Task *pyllbc_TaskMgr::GetTask(int taskId)
{
    LLBC_Guard guard(_lock);
    _Tasks::iterator it = _tasks.find(taskId);

    return it != _tasks.end() ? it->second : NULL;
}

int pyllbc_TaskMgr::GetCurrentTaskId() const
{
//    __LLBC_LibTls *tls = __LLBC_GetLibTls();
//    if (tls->coreTls.entryThread)
//        return 0;
//    else if (tls->coreTls.task && !tls->commTls.service)
//        return (static_cast<pyllbc_Task *>(tls->coreTls.task))->GetId();
//
//    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
//    return LLBC_FAILED;

    // TODO: Do stuff here.

    pyllbc_SetError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

bool pyllbc_TaskMgr::IsTaskExist(int taskId) const
{
    This *ncThis = const_cast<This *>(this);
    return !!ncThis->GetTask(taskId);
}

bool pyllbc_TaskMgr::IsTaskActivated(int taskId) const
{
    // TODO: Do stuff here.
    return false;
}

uint32 pyllbc_TaskMgr::GetTaskCount() const
{
    This *ncThis = const_cast<This *>(this);

    LLBC_Guard(ncThis->_lock);
    return static_cast<uint32>(ncThis->_tasks.size());
}

int pyllbc_TaskMgr::PushMsg(int taskId, LLBC_MessageBlock *msg)
{
    if (UNLIKELY(!msg))
    {
        pyllbc_SetError("msg null", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (taskId == 0)
    {
        _mainThreadQueue.PushBack(msg);
        return LLBC_OK;
    }

    LLBC_Guard guard(_lock);
    _Tasks::iterator it = _tasks.find(taskId);
    if (it == _tasks.end())
    {
        pyllbc_SetError("task not exist", LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    it->second->Push(msg);
    return LLBC_OK;
}

int pyllbc_TaskMgr::PopMsg(int taskId, LLBC_MessageBlock *&msg, int interval)
{
    if (taskId == 0)
    {
        if (interval == 0)
        {
            return _mainThreadQueue.TryPopFront(msg);
        }
        else if (interval == LLBC_INFINITE)
        {
            _mainThreadQueue.PopFront(msg);
            return LLBC_OK;
        }

        return _mainThreadQueue.TimedPopFront(msg, interval);
    }

    LLBC_Guard guard(_lock);
    _Tasks::iterator it = _tasks.find(taskId);
    if (it == _tasks.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    pyllbc_Task *task = it->second;
    if (interval == 0)
        return task->TryPop(msg);
    else if (interval == LLBC_INFINITE)
        return task->Pop(msg);

    return task->TimedPop(msg, interval);
}

void pyllbc_TaskMgr::OnTaskCreated(pyllbc_Task *task)
{
}

void pyllbc_TaskMgr::OnTaskDestroy(pyllbc_Task *task)
{
    LLBC_Guard guard(_lock);
    _tasks.erase(task->GetId());

    delete task;
}
