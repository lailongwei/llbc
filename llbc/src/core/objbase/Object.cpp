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

#include "llbc/common/Config.h"

#include "llbc/core/os/OS_Atomic.h"

#include "llbc/core/objbase/Object.h"
#include "llbc/core/objbase/AutoReleasePoolStack.h"

__LLBC_NS_BEGIN

LLBC_Object::LLBC_Object()
: _ref(1)
, _autoRef(0)

, _poolStack(nullptr)
{
}

LLBC_Object::~LLBC_Object()
{
    if (_autoRef && _poolStack)
    {
        _poolStack->RemoveObject(this);
    }
}

int LLBC_Object::GetRefCount()
{
    return _ref;
}

int LLBC_Object::GetAutoRefCount()
{
    return _autoRef;
}

void LLBC_Object::Release()
{
    if (--_ref == 0)
        delete this;
}

void LLBC_Object::SafeRetain()
{
    (void)LLBC_AtomicFetchAndAdd(&_ref, 1);
}

void LLBC_Object::SafeRelease()
{
    if (LLBC_AtomicFetchAndSub(&_ref, 1) == 1)
        delete this;
}

void LLBC_Object::Retain()
{
    ++_ref;
}

int LLBC_Object::AutoRelease()
{
    if (!_poolStack)
    {
        __LLBC_LibTls *tls = __LLBC_GetLibTls();
        _poolStack = reinterpret_cast<
            LLBC_AutoReleasePoolStack *>(tls->objbaseTls.poolStack);
    }

    return _poolStack->AddObject(this);
}

LLBC_Object *LLBC_Object::Clone() const
{
    return new LLBC_Object;
}

LLBC_String LLBC_Object::ToString() const
{
    LLBC_String str;
    return str.append("Object: ")
              .append_format("%x", this);
}

void LLBC_Object::Serialize(LLBC_Stream &s) const
{
    // Do nothing.
    // ... ...
}

bool LLBC_Object::DeSerialize(LLBC_Stream &s)
{
    // Do nothing.
    // ... ...

    return true;
}

__LLBC_NS_END
