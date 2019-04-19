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

static const std::map<int, LLBC_NS LLBC_Variant> __emptyIntKeyParams;
static const std::map<LLBC_NS LLBC_String, LLBC_NS LLBC_Variant> __emptyStrKeyParams;
static const std::map<const char*, LLBC_NS LLBC_Variant> __emptyConstantStrKeyParams;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Event::LLBC_Event(int id, bool dontDelAfterFire)
: _id(id)
, _dontDelAfterFire(dontDelAfterFire)
, _intKeyParams(NULL)
, _constantStrKeyParams(NULL)
, _strKeyParams(NULL)
{
}

LLBC_Event::~LLBC_Event()
{
    if (_intKeyParams)
        LLBC_Delete(_intKeyParams);

    if (_constantStrKeyParams)
        LLBC_Delete(_constantStrKeyParams);

    if (_strKeyParams)
        LLBC_Delete(_strKeyParams);
}

const LLBC_Variant &LLBC_Event::GetParam(int key) const
{
    if (_intKeyParams == NULL)
        return LLBC_INL_NS __nilVariant;

    _IntKeyParams::const_iterator it = _intKeyParams->find(key);
    return it != _intKeyParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

const LLBC_Variant &LLBC_Event::GetParam(const char* key) const
{
    if (_constantStrKeyParams == NULL)
        return LLBC_INL_NS __nilVariant;

    _ConstantStrKeyParams::const_iterator it = _constantStrKeyParams->find(key);
    return it != _constantStrKeyParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

const LLBC_Variant &LLBC_Event::GetParam(const LLBC_String &key) const
{
    if (_strKeyParams == NULL)
        return LLBC_INL_NS __nilVariant;

    _StrKeyParams::const_iterator it = _strKeyParams->find(key);
    return it != _strKeyParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

LLBC_Event &LLBC_Event::SetParam(int key, const LLBC_Variant &param)
{
    if (_intKeyParams == NULL)
        _intKeyParams = LLBC_New0(_IntKeyParams);

    _IntKeyParams::iterator it = _intKeyParams->find(key);
    if (it == _intKeyParams->end())
        _intKeyParams->insert(std::make_pair(key, param));
    else
        it->second = param;

    return *this;
}

LLBC_Event &LLBC_Event::SetParam(const char* key, const LLBC_Variant &param)
{
    if (_constantStrKeyParams == NULL)
        _constantStrKeyParams = LLBC_New0(_ConstantStrKeyParams);

    _ConstantStrKeyParams::iterator it = _constantStrKeyParams->find(key);
    if (it == _constantStrKeyParams->end())
        _constantStrKeyParams->insert(std::make_pair(key, param));
    else
        it->second = param;

    return *this;
}

LLBC_Event &LLBC_Event::SetParam(const LLBC_String &key, const LLBC_Variant &param)
{
    if (_strKeyParams == NULL)
        _strKeyParams = LLBC_New0(_StrKeyParams);

    _StrKeyParams::iterator it = _strKeyParams->find(key);
    if (it == _strKeyParams->end())
        _strKeyParams->insert(std::make_pair(key, param));
    else
        it->second = param;

    return *this;
}

const std::map<int, LLBC_Variant> &LLBC_Event::GetIntKeyParams() const
{
    return _intKeyParams != NULL ? *_intKeyParams: LLBC_INL_NS __emptyIntKeyParams;
}

const std::map<const char*, LLBC_Variant> &LLBC_Event::GetConstantStrKeyParams() const
{
    return _constantStrKeyParams != NULL ? *_constantStrKeyParams : LLBC_INL_NS __emptyConstantStrKeyParams;
}

const std::map<LLBC_String, LLBC_Variant> &LLBC_Event::GetStrKeyParams() const
{
    return _strKeyParams != NULL ? *_strKeyParams : LLBC_INL_NS __emptyStrKeyParams;
}

LLBC_Variant &LLBC_Event::operator [](int key)
{
    if (!_intKeyParams)
        _intKeyParams = LLBC_New0(_IntKeyParams);

    _IntKeyParams::iterator it = _intKeyParams->find(key);
    if (it == _intKeyParams->end())
        return _intKeyParams->insert(std::make_pair(key, LLBC_Variant())).first->second;
    else
        return it->second;
}

LLBC_Variant &LLBC_Event::operator [](const char *key)
{
    if (!_constantStrKeyParams)
        _constantStrKeyParams = LLBC_New0(_ConstantStrKeyParams);

    _ConstantStrKeyParams::iterator it = _constantStrKeyParams->find(key);
    if (it == _constantStrKeyParams->end())
        return _constantStrKeyParams->insert(std::make_pair(key, LLBC_Variant())).first->second;
    else
        return it->second;
}

LLBC_Variant &LLBC_Event::operator [](const LLBC_String &key)
{
    if (!_strKeyParams)
        _strKeyParams = LLBC_New0(_StrKeyParams);

    _StrKeyParams::iterator it = _strKeyParams->find(key);
    if (it == _strKeyParams->end())
        return _strKeyParams->insert(std::make_pair(key, LLBC_Variant())).first->second;
    else
        return it->second;
}

const LLBC_Variant &LLBC_Event::operator [](int key) const
{
    if (!_intKeyParams)
        return LLBC_INL_NS __nilVariant;

    _IntKeyParams::const_iterator it = _intKeyParams->find(key);
    return it != _intKeyParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

const LLBC_Variant &LLBC_Event::operator [](const char* key) const
{
    if (!_constantStrKeyParams)
        return LLBC_INL_NS __nilVariant;

    _ConstantStrKeyParams::const_iterator it = _constantStrKeyParams->find(key);
    return it != _constantStrKeyParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

const LLBC_Variant &LLBC_Event::operator [](const LLBC_String &key) const
{
    if (!_strKeyParams)
        return LLBC_INL_NS __nilVariant;

    _StrKeyParams::const_iterator it = _strKeyParams->find(key);
    return it != _strKeyParams->end() ? it->second : LLBC_INL_NS __nilVariant;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
