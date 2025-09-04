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

#include "llbc/core/log/LogEnvToken.h"

__LLBC_NS_BEGIN

int LLBC_LogEnvToken::Initialize(const LLBC_LogFormattingInfo &formatter,
                                 const LLBC_LogTimeAccessor &logTimeAccessor,
                                 const LLBC_String &str)
{
    SetFormatter(formatter);
    if (!formatter.addiParam.empty())
    {
        const char *envName = formatter.addiParam.c_str();
        #if LLBC_TARGET_PLATFORM_WIN32
        char *envVal;
        size_t envValSize;
        if (_dupenv_s(&envVal, &envValSize, envName) == 0 && envVal != nullptr)
        {
            _envValue.append(envVal);
            free(envVal);
        }
        #else // Non-Win32
        // getenv_s() require C11 support.
        // size_t requireSize;
        // if (getenv_s(&requireSize, nullptr, 0, envName) == 0 && requireSize > 0)
        // {
        //     char *envVal = LLBC_Malloc(char, requireSize);
        //     LLBC_Defer(free(envVal));

        //     if (getenv_s(&requireSize, envVal, requireSize, envName) == 0 && requireSize > 0)
        //         _envValue.append(envVal);
        // }

        char *envVal = getenv(envName);
        if (envVal)
            _envValue.append(envVal);
        #endif // Win32
    }

    return LLBC_OK;
}

int LLBC_LogEnvToken::GetType() const
{
    return LLBC_LogTokenType::EnvToken;
}

void LLBC_LogEnvToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    if (!_envValue.empty())
    {
        const int index = static_cast<int>(formattedData.size());

        formattedData.append(_envValue);
        GetFormatter().Format(formattedData, index);
    }
}

__LLBC_NS_END
