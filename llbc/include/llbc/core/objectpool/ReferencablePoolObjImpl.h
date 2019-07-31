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

#ifdef __LLBC_CORE_OBJECT_POOL_REFERENCABLE_POOL_OBJ_H__

#include "llbc/core/os/OS_Atomic.h"
#include "llbc/core/objectpool/IObjectPoolInst.h"

__LLBC_NS_BEGIN

inline LLBC_ReferencablePoolObj::LLBC_ReferencablePoolObj()
: _poolInst(NULL)
{
}

inline LLBC_ReferencablePoolObj::~LLBC_ReferencablePoolObj()
{
}

inline void LLBC_ReferencablePoolObj::Release()
{
    if (--_ref == 0)
    {
        if (_poolInst)
        {
            _ref = 1;
            _autoRef = 0;
            _poolStack = NULL;

            _poolInst->ReleaseReferencable(this);
        }
        else
        {
            LLBC_Delete(this);
        }
    }
}

inline void LLBC_ReferencablePoolObj::SafeRelease()
{
    if (LLBC_AtomicFetchAndSub(&_ref, 1) == 1)
    {
        if (_poolInst)
        {
            _ref = 1;
            _autoRef = 0;
            _poolStack = NULL;

            _poolInst->ReleaseReferencable(this);
        }
        else
        {
            LLBC_Delete(this);
        }
    }
}

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_POOL_REFERENCABLE_POOL_OBJ_H__
