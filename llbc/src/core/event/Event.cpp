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

#include "llbc/core/event/Event.h"
#include "llbc/common/Macro.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_Variant __nilVariant;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Event::LLBC_Event(int id, bool dontDelAfterFire)
: _id(id)
, _dontDelAfterFire(dontDelAfterFire)
, _params({})
, _extData(nullptr)
, _extDataClearDeleg(nullptr) {}

LLBC_Event::~LLBC_Event()
{
    ClearExtData();
}

const LLBC_Variant &LLBC_Event::GetParam(const LLBC_Variant &key) const
{
    const auto it = _params.find(key);
    return it != _params.end() ? it->second : LLBC_INL_NS __nilVariant;
}

LLBC_Event &LLBC_Event::SetParam(const LLBC_Variant &key, const LLBC_Variant &param)
{
    if (const auto it = _params.find(key); it == _params.end())
        _params.insert(std::make_pair(key, param));
    else
        it->second = param;

    return *this;
}

const std::map<LLBC_Variant, LLBC_Variant> &LLBC_Event::GetParams() const
{
    return _params;
}

std::map<LLBC_Variant, LLBC_Variant> &LLBC_Event::GetMutableParams()
{
    return _params;
}

LLBC_Event * LLBC_Event::Clone() const
{
    auto *clone = new LLBC_Event(_id, false);
    clone->_params = _params;

    return clone;
} 

LLBC_Variant &LLBC_Event::operator[](const LLBC_Variant &key)
{
    const auto it = _params.find(key);
    return it == _params.end() ? _params.insert(std::make_pair(key, LLBC_Variant())).first->second : it->second;
}

const LLBC_Variant &LLBC_Event::operator[](const LLBC_Variant &key) const
{
    const auto it = _params.find(key);
    return it != _params.end() ? it->second : LLBC_INL_NS __nilVariant;
}

void LLBC_Event::Reuse()
{
    _id = 0;
    _dontDelAfterFire = false;
    _params.clear();
    ClearExtData();
}

__LLBC_NS_END
