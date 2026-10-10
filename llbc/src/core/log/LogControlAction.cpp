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

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogControl.h"
#include "llbc/core/log/LogControlExecutor.h"
#include "llbc/core/log/LogControlAction.h"

__LLBC_NS_BEGIN

int LLBC_LogMuteAction::Initialize(const LLBC_LogControlItem &item)
{
    return LLBC_OK;
}

void LLBC_LogMuteAction::Apply(ApplyResult &io) const
{
    // Caller (LogControlMgr::Apply) breaks the chain and bumps the
    // suppression counter on muted.
    io.muted = true;
}

LLBC_LogSetLevelAction::LLBC_LogSetLevelAction()
: _newLevel(LLBC_LogLevel::End)
{
}

int LLBC_LogSetLevelAction::Initialize(const LLBC_LogControlItem &item)
{
    if (!LLBC_LogLevel::IsValid(item.newLevel))
        return LLBC_FAILED;

    _newLevel = item.newLevel;
    return LLBC_OK;
}

void LLBC_LogSetLevelAction::Apply(ApplyResult &io) const
{
    io.effectiveLevel = _newLevel;
}

LLBC_BaseLogControlAction *LLBC_BaseLogControlAction::Create(const LLBC_LogControlItem &item)
{
    LLBC_BaseLogControlAction *action = nullptr;
    switch (item.action)
    {
    case LLBC_LogControlAction::Mute:
        action = new LLBC_LogMuteAction;
        break;

    case LLBC_LogControlAction::SetLevel:
        action = new LLBC_LogSetLevelAction;
        break;

    default:
        // Includes LLBC_LogControlAction::Unset and out-of-range values.
        return nullptr;
    }

    if (action->Initialize(item) != LLBC_OK)
    {
        LLBC_XDelete(action);
        return nullptr;
    }

    return action;
}

__LLBC_NS_END
