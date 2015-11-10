/**
 * @file    Object.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/14
 * @version 1.0
 *
 * @bfief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/objbase/Object.h"
#include "llbc/objbase/AutoReleasePoolStack.h"

__LLBC_NS_BEGIN

LLBC_Object::LLBC_Object()
: _ref(1)
, _autoRef(0)

, _poolStack(NULL)
{
}

LLBC_Object::~LLBC_Object()
{
    if (_autoRef && _poolStack)
    {
        _poolStack->RemoveObject(this);
    }
}

void LLBC_Object::Release()
{
    if (-- _ref == 0)
    {
        delete this;
    }
}

void LLBC_Object::Retain()
{
    ++ _ref;
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

void LLBC_Object::SerializeEx(LLBC_Stream &s) const
{
    // Do nothing.
    // ... ...
}

bool LLBC_Object::DeSerializeEx(LLBC_Stream &s)
{
    // Do nothing.
    // ... ...

    return true;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
