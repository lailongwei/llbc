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

#include "llbc/core/objectpool/IObjectPool.h"
#include "llbc/core/objectpool/IObjectPoolInstFactory.h"

__LLBC_NS_BEGIN

LLBC_SpinLock LLBC_IObjectPool::_poolInstFactoryLock;
std::map<LLBC_CString, LLBC_IObjectPoolInstFactory *> LLBC_IObjectPool::_poolInstFactories;

int LLBC_IObjectPool::RegisterPoolInstFactory(LLBC_IObjectPoolInstFactory *instFactory)
{
    if (UNLIKELY(!instFactory))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _poolInstFactoryLock.Lock();
    std::map<LLBC_CString, LLBC_IObjectPoolInstFactory *>::iterator it = _poolInstFactories.find(instFactory->GetName());
    if (it != _poolInstFactories.end())
    {
        _poolInstFactoryLock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_REPEAT);

        return LLBC_FAILED;
    }

    _poolInstFactories.insert(std::make_pair(instFactory->GetName(), instFactory));
    _poolInstFactoryLock.Unlock();

    return LLBC_OK;
}

void LLBC_IObjectPool::DestroyAllPoolInstFactories()
{
    _poolInstFactoryLock.Lock();
    for (std::map<LLBC_CString, LLBC_IObjectPoolInstFactory *>::iterator it = _poolInstFactories.begin();
         it != _poolInstFactories.end();
         ++it)
        delete it->second;

    _poolInstFactories.clear();

    _poolInstFactoryLock.Unlock();
}

__LLBC_NS_END
