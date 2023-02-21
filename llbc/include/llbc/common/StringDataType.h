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

#pragma once

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/BasicString.h"
#include "llbc/common/BasicCString.h"

__LLBC_NS_BEGIN

typedef LLBC_BasicString<char> LLBC_String;
typedef LLBC_BasicString<wchar> LLBC_WString;
typedef std::vector<LLBC_BasicString<char> > LLBC_Strings;
typedef std::vector<LLBC_BasicString<wchar> > LLBC_WStrings;

typedef LLBC_BasicCString<char> LLBC_CString;
typedef LLBC_BasicCString<wchar> LLBC_CWString;

#ifdef LLBC_UNICODE
 typedef LLBC_WString LLBC_TString;
 typedef LLBC_WStrings LLBC_TStrings;
 typedef LLBC_CWString LLBC_CTString;
#else // !LLBC_UNICODE
 typedef LLBC_String LLBC_TString;
 typedef LLBC_Strings LLBC_TStrings;
 typedef LLBC_CString LLBC_TCString;
#endif // LLBC_UNICODE

__LLBC_NS_END
