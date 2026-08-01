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

#include "llbc/core/thread/Guard.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogControlMgr.h"

__LLBC_NS_BEGIN

LLBC_LogControlMgr::LLBC_LogControlMgr()
: _lock()
, _execs(nullptr)
, _suppressedCount(0)
{
}

int LLBC_LogControlMgr::SetControls(const std::vector<LLBC_LogControlItem> &items)
{
    if (items.empty())
    {
        LLBC_LockGuard guard(_lock);
        _execs.reset();
        return LLBC_OK;
    }

    // Build outside the lock; on any invalid item the previously published
    // snapshot stays in effect.
    auto next = std::make_shared<ExecutorList>();
    next->reserve(items.size());
    for (const auto &item : items)
    {
        std::unique_ptr<LLBC_LogControlExecutor> exec(LLBC_LogControlExecutor::Create(item));
        if (!exec)
            return LLBC_FAILED;
        next->push_back(std::move(exec));
    }

    LLBC_LockGuard guard(_lock);
    _execs = std::move(next);
    return LLBC_OK;
}

size_t LLBC_LogControlMgr::GetControlCount() const
{
    LLBC_LockGuard guard(_lock);
    return _execs ? _execs->size() : 0;
}

void LLBC_LogControlMgr::GetControls(std::vector<LLBC_LogControlItem> &out) const
{
    out.clear();

    // Snapshot under the lock, traverse outside.
    std::shared_ptr<const ExecutorList> snap;
    {
        LLBC_LockGuard guard(_lock);
        snap = _execs;
    }

    if (!snap)
        return;
    out.reserve(snap->size());
    for (const auto &e : *snap)
        out.push_back(e->GetItem());
}

uint64 LLBC_LogControlMgr::GetSuppressedCount() const
{
    LLBC_LockGuard guard(_lock);
    return _suppressedCount;
}

void LLBC_LogControlMgr::ResetSuppressedCount()
{
    LLBC_LockGuard guard(_lock);
    _suppressedCount = 0;
}

bool LLBC_LogControlMgr::IsEmpty() const
{
    LLBC_LockGuard guard(_lock);
    return !_execs || _execs->empty();
}

LLBC_LogControlMgr::ApplyResult LLBC_LogControlMgr::Apply(int appenderType,
                                                          const LLBC_LogData &data,
                                                          int originalLevel) const
{
    ApplyResult r{ false, originalLevel };

    // Snapshot under the lock, traverse outside; the shared_ptr copy pins
    // the snapshot for the rest of this call.
    std::shared_ptr<const ExecutorList> snap;
    {
        LLBC_LockGuard guard(_lock);
        snap = _execs;
    }

    if (!snap || snap->empty())
        return r;

    // Declaration-order traversal; Match() runs against the current effective
    // level so chained SetLevel actions are observable to later executors.
    // Mute short-circuits the chain.
    for (const auto &exec : *snap)
    {
        if (!exec->Match(appenderType, data, r.effectiveLevel))
            continue;

        exec->Action(r);
        if (r.muted)
        {
            LLBC_LockGuard guard(_lock);
            ++_suppressedCount;
            return r;
        }
    }

    return r;
}

__LLBC_NS_END
