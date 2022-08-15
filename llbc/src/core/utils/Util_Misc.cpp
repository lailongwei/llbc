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

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/utils/Util_Misc.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_Variant __nilVariant;

static std::vector<LLBC_NS LLBC_Variant> __emptySeqArgs;
static std::map<LLBC_NS LLBC_String, LLBC_NS LLBC_Variant> __emptyNamingArgs;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_StartArgs::LLBC_StartArgs()
: _parsed(false)
{
}

LLBC_StartArgs::~LLBC_StartArgs()
{
}

int LLBC_StartArgs::Parse(int argc, char *argv[])
{
    if (argc < 1)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (_parsed)
    {
        _moduleFilePath.clear();

        _seqArgs.clear();
        _namingArgs.clear();

        _parsed = false;
    }

    _moduleFilePath = argv[0];
    for (int i = 1; i < argc; ++i)
    {
        LLBC_String arg = argv[i];
        _seqArgs.push_back(LLBC_Variant(arg));

        size_t idx = arg.find('=');
        if (idx  == LLBC_String::npos || idx == 0)
            continue;

        LLBC_Strings splitedArg = arg.split('=', 1);
        _namingArgs[splitedArg[0]] = splitedArg[1];
    }

    return LLBC_OK;
}

bool LLBC_StartArgs::IsParsed() const
{
    return _parsed;
}

const LLBC_String &LLBC_StartArgs::GetModuleFilePath() const
{
    return _moduleFilePath;
}

size_t LLBC_StartArgs::GetArgumentsCount() const
{
    return _seqArgs.size();
}

const std::vector<LLBC_Variant> &LLBC_StartArgs::GetAllArguments() const
{
    return _seqArgs;
}

size_t LLBC_StartArgs::GetNamingArgumentsCount() const
{
    return _namingArgs.size();
}

const std::map<LLBC_String, LLBC_Variant> &LLBC_StartArgs::GetAllNamingArguments() const
{
    return _namingArgs;
}

const LLBC_Variant &LLBC_StartArgs::operator [](size_t index) const
{
    return index < _seqArgs.size() ? 
        _seqArgs.at(index) : (LLBC_INL_NS __nilVariant);
}

const LLBC_Variant &LLBC_StartArgs::operator [](const LLBC_String &key) const
{
    _NamingArgs::const_iterator it = _namingArgs.find(key);
    return it != _namingArgs.end() ? it->second : (LLBC_INL_NS __nilVariant);
}

__LLBC_NS_END
