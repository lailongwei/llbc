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

#include "llbc/comm/ServiceImpl.h"

__LLBC_NS_BEGIN

LLBC_Service *LLBC_Service::Create(const LLBC_String &name,
                                   LLBC_IProtocolFactory *dftProtocolFactory,
                                   bool fullStack)
{
    return new LLBC_ServiceImpl(name, dftProtocolFactory, fullStack);
}

void LLBC_Service::GetCompName(const char *qualifiedCompName,
                               char(&compName)[LLBC_CFG_COMM_MAX_COMP_NAME_LEN + 1],
                               size_t &compNameLen)
{
    // Demangle.
    #if LLBC_TARGET_PLATFORM_WIN32
    compNameLen = strlen(qualifiedCompName);
    const char *demangledCompName = qualifiedCompName;
    #else // Non-Win32
    int demangleStatus;
    size_t bufLen = __LLBC_RTTI_BUF_SIZE;
    char *demangledCompName = __LLBC_GetLibTls()->commonTls.rtti;
    abi::__cxa_demangle(qualifiedCompName,
                        demangledCompName,
                        &bufLen,
                        &demangleStatus);
    if (UNLIKELY(demangleStatus != 0))
        demangledCompName = const_cast<char *>(qualifiedCompName);
    compNameLen = strlen(demangledCompName);
    #endif // Win32

    // Trim namespace(s) prefix.
    const char *compNameBeg = demangledCompName + compNameLen - 1;
    while (compNameBeg != demangledCompName)
    {
        if (*--compNameBeg == ':')
        {
            ++compNameBeg;
            break;
        }
    }

    // Normalize comp name length.
    compNameLen -= (compNameBeg - demangledCompName);

    // Copy to output param: compName.
    if (UNLIKELY(compNameLen > LLBC_CFG_COMM_MAX_COMP_NAME_LEN))
    {
        compNameLen = LLBC_CFG_COMM_MAX_COMP_NAME_LEN;
        memcpy(compName, compNameBeg, LLBC_CFG_COMM_MAX_COMP_NAME_LEN);
        compName[LLBC_CFG_COMM_MAX_COMP_NAME_LEN] = '\0';
    }
    else
    {
        memcpy(compName, compNameBeg, compNameLen + 1);
    }
}

void LLBC_Service::SetEventInfo(LLBC_ServiceEventFirer *eventServiceFirer, LLBC_Event *ev)
{
    if(UNLIKELY(eventServiceFirer == nullptr))
        return ;

    eventServiceFirer->SetEventInfo(ev, this);
}

__LLBC_NS_END
