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

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/ScriptIntegrator.h"

pyllbc_ScriptIntegrator::pyllbc_ScriptIntegrator(const LLBC_String &script)
: _script(script)
, _builded(false)
{
    ASSERT(!_script.empty() &&
        "python script content must not null!");
}

pyllbc_ScriptIntegrator::~pyllbc_ScriptIntegrator()
{
}

int pyllbc_ScriptIntegrator::Integrate()
{
    if (_builded)
        return LLBC_OK;

    if (PyRun_SimpleString(_script.c_str()) != 0)
    {
        pyllbc_SetError(PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    _builded = true;

    return LLBC_OK;
}
