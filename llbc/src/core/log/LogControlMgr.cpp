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
: _execs(nullptr)
, _empty(true)
, _suppressedCount(0)
{
}

int LLBC_LogControlMgr::SetControls(const std::vector<LLBC_LogControlItem> &items)
{
    // Build (and validate) every executor up front, entirely outside the
    // write lock. On any validation failure we leave the published snapshot
    // untouched, so callers (Logger::Initialize / LoggerMgr::Reload) can
    // either keep the previous configuration alive or surface the error to
    // the user without having corrupted runtime state. Performing the full
    // build before locking also keeps the critical section to just the
    // atomic publish (no allocation under lock).
    auto next = std::make_shared<ExecutorList>();
    next->reserve(items.size());
    for (const auto &item : items)
    {
        std::unique_ptr<LLBC_LogControlExecutor> exec(LLBC_LogControlExecutor::Create(item));
        if (!exec)
            return LLBC_FAILED;
        next->push_back(std::move(exec));
    }

    // Serialize concurrent writers (e.g. overlapping Reload()s). Readers do
    // NOT take this lock; they consume the atomic shared_ptr.
    LLBC_LockGuard guard(_writeLock);
    if (items.empty())
    {
        // Publish a null snapshot; readers loading it treat it as empty.
        // Keeping the null form (rather than an empty vector) lets the
        // Apply() fast-path short-circuit on the nullptr check alone.
        std::atomic_store_explicit(
            &_execs,
            std::shared_ptr<const ExecutorList>(),
            std::memory_order_release);
        _empty = true;
    }
    else
    {
        std::atomic_store_explicit(
            &_execs,
            std::shared_ptr<const ExecutorList>(next),
            std::memory_order_release);
        _empty = false;
    }
    return LLBC_OK;
}

size_t LLBC_LogControlMgr::GetControlCount() const
{
    auto snap = std::atomic_load_explicit(&_execs, std::memory_order_acquire);
    return snap ? snap->size() : 0;
}

void LLBC_LogControlMgr::GetControls(std::vector<LLBC_LogControlItem> &out) const
{
    out.clear();
    auto snap = std::atomic_load_explicit(&_execs, std::memory_order_acquire);
    if (!snap)
        return;
    out.reserve(snap->size());
    for (const auto &e : *snap)
        out.push_back(e->GetItem());
}

LLBC_LogControlMgr::ApplyResult LLBC_LogControlMgr::Apply(int appenderType,
                                                          const LLBC_LogData &data,
                                                          int originalLevel) const
{
    ApplyResult r{ false, originalLevel };

    // Lock-free read: take a reference to the current immutable snapshot.
    // Concurrent writers may publish a new snapshot; this Apply() will keep
    // running on the captured one until it returns.
    auto snap = std::atomic_load_explicit(&_execs, std::memory_order_acquire);
    if (!snap || snap->empty())
        return r;

    // Pipeline: declaration-order traversal; each executor decides whether it
    // fires (scope + match rules) and what it does (Mute / SetLevel). Mute
    // short-circuits the chain and the suppression counter is bumped here —
    // Mgr owns chain-level signals; the executor itself does not see the
    // global counter (see LLBC_LogControlExecutor docs).
    for (const auto &exec : *snap)
    {
        // Match is evaluated against the *current* effective level so chained
        // SetLevel actions are observable to subsequent executors.
        if (!exec->Match(appenderType, data, r.effectiveLevel))
            continue;

        exec->Action(r);
        if (r.muted)
        {
            _suppressedCount.fetch_add(1, std::memory_order_relaxed);
            return r;
        }
    }

    return r;
}

__LLBC_NS_END
