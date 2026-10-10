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

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/BaseLogAppender.h"
#include "llbc/core/log/LogMatchRule.h"
#include "llbc/core/log/LogControlAction.h"
#include "llbc/core/log/LogControlExecutor.h"

__LLBC_NS_BEGIN

LLBC_LogControlExecutor::LLBC_LogControlExecutor(
    const LLBC_LogControlItem &item,
    std::vector<LLBC_BaseLogMatchRule *> rules,
    LLBC_BaseLogControlAction *action)
: _item(item)
, _rules(std::move(rules))
, _action(action)
{
}

LLBC_LogControlExecutor::~LLBC_LogControlExecutor()
{
    for (auto *rule : _rules)
        delete rule;
    _rules.clear();

    LLBC_XDelete(_action);
}

LLBC_LogControlExecutor *LLBC_LogControlExecutor::FailCreate(
    std::vector<LLBC_BaseLogMatchRule *> &rules,
    LLBC_BaseLogControlAction *&action)
{
    for (auto *r : rules)
        delete r;
    rules.clear();
    LLBC_XDelete(action);
    LLBC_SetLastError(LLBC_ERROR_INVALID);
    return nullptr;
}

LLBC_LogControlExecutor *LLBC_LogControlExecutor::Create(const LLBC_LogControlItem &item)
{
    // Single error-reporting boundary: rule/action factories return nullptr
    // on failure; any failure jumps to FailCreate() which cleans up partial
    // builds and sets LLBC_ERROR_INVALID.
    std::vector<LLBC_BaseLogMatchRule *> rules;
    rules.reserve(4);
    LLBC_BaseLogControlAction *action = nullptr;

    // 1) at least one match rule must be enabled.
    if (!item.HasAnyMatch())
        return FailCreate(rules, action);

    // 2) build a rule for every enabled match dimension (AND-combined).
    for (int mt = LLBC_LogControlMatchType::Begin;
         mt < LLBC_LogControlMatchType::End;
         ++mt)
    {
        if (!LLBC_BaseLogMatchRule::IsEnabled(mt, item))
            continue;

        auto *r = LLBC_BaseLogMatchRule::Create(mt, item);
        if (!r)
            return FailCreate(rules, action);
        rules.push_back(r);
    }

    // 3) action.
    action = LLBC_BaseLogControlAction::Create(item);
    if (!action)
        return FailCreate(rules, action);

    // 4) appender scope: reject any unknown appender type. The actual scope
    //    check is inlined in Match() because it operates on appenderType
    //    rather than on LogData.
    for (int t : item.appenderTypes)
    {
        if (!LLBC_LogAppenderType::IsValid(t))
            return FailCreate(rules, action);
    }

    return new LLBC_LogControlExecutor(item, std::move(rules), action);
}

bool LLBC_LogControlExecutor::Match(int appenderType,
                                    const LLBC_LogData &data,
                                    int currentLevel) const
{
    // Appender scope: empty == all appenders. Inlined here (instead of a
    // dedicated rule subclass) because it operates on appenderType.
    if (!_item.appenderTypes.empty())
    {
        bool scopeHit = false;
        for (int t : _item.appenderTypes)
        {
            if (t == appenderType)
            {
                scopeHit = true;
                break;
            }
        }
        if (!scopeHit)
            return false;
    }

    // Rules are AND-combined; `_rules` is never empty (Create() guarantees).
    for (const auto *rule : _rules)
    {
        if (!rule->Match(data, currentLevel))
            return false;
    }

    return true;
}

void LLBC_LogControlExecutor::Action(ApplyResult &io) const
{
    _action->Apply(io);
}

__LLBC_NS_END
