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

#include "llbc/core/event/Event.h"
#include "llbc/common/Macro.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_Variant __nilVariant;

static const std::vector<LLBC_NS LLBC_Variant> __emptySeqParams;
static const std::map<LLBC_NS LLBC_String, LLBC_NS LLBC_Variant> __emptyNamingParams;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Event::LLBC_Event(int id)
: _id(id)
, _seqParams(NULL)
, _namingParams(NULL)
{
}

LLBC_Event::~LLBC_Event()
{
    LLBC_XDelete(_seqParams);
    LLBC_XDelete(_namingParams);
}

int LLBC_Event::GetId() const
{
    return _id;
}

LLBC_Event &LLBC_Event::AddParam(const LLBC_Variant &param)
{
    if (_seqParams == NULL)
        _seqParams = new _SeqParams();

    _seqParams->push_back(param);
    return *this;
}

LLBC_Event &LLBC_Event::AddParam(const LLBC_String &key, const LLBC_Variant &param)
{
    if (_namingParams == NULL)
        _namingParams = new _NamingParams();

    _namingParams->insert(std::make_pair(key, param));
    return *this;
}

const std::vector<LLBC_Variant> &LLBC_Event::GetSequentialParams() const
{
    return _seqParams != NULL ? *_seqParams : LLBC_INL_NS __emptySeqParams;
}

size_t LLBC_Event::GetSequentialParamsCount() const
{
    return _seqParams != NULL ? _seqParams->size() : 0;
}

const std::map<LLBC_String, LLBC_Variant> &LLBC_Event::GetNamingParams() const
{
    return _namingParams != NULL ? *_namingParams : LLBC_INL_NS __emptyNamingParams;
}

size_t LLBC_Event::GetNamingParamsCount() const
{
    return _namingParams != NULL ? _namingParams->size() : 0;
}

const LLBC_Variant &LLBC_Event::operator [](size_t index) const
{
    if (!_seqParams || 
            (index < 0 || index >= _seqParams->size()))
        return LLBC_INL_NS __nilVariant;

    return _seqParams->at(index);
}

const LLBC_Variant &LLBC_Event::operator [](const LLBC_String &key) const
{
    if (!_namingParams)
        return LLBC_INL_NS __nilVariant;

    _NamingParams::const_iterator it = _namingParams->find(key);
    return it != _namingParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
