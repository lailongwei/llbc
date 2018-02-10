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

#ifndef __LLBC_OBJBASE_OBJECT_H__
#define __LLBC_OBJBASE_OBJECT_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_AutoReleasePool;
class LLBC_AutoReleasePoolStack;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The llbc library class encapsulation.
 */
class LLBC_EXPORT LLBC_Object
{
public:
    LLBC_Object();
    virtual ~LLBC_Object();

public:
    /**
     * Retain object.
     */
    virtual void Retain();

    /**
     * Release object.
     */
    virtual void Release();

    /**
     * Auto release object.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int AutoRelease();

    /**
     * Clone object.
     * @return LLBC_Object * - the new object.
     */
    virtual LLBC_Object *Clone() const;

    /**
     * To string method.
     * @return LLBC_String - object describe.
     */
    virtual LLBC_String ToString() const;

public:
    /**
     * Serialize method, use to serialize the object to stream.
     * @param[out] s - the stream object.
     */
    virtual void Serialize(LLBC_Stream &s) const;

    /**
     * DeSerialize method, use to deserialize the object from stream.
     * @param[in] s - the stream object.
     * @return bool - return true if serialize success, otherwise return false.
     */
    virtual bool DeSerialize(LLBC_Stream &s);

    /**
     * Extensional serialize method, use to serialize the object to stream.
     * @param[out] s - the stream object.
     */
    virtual void SerializeEx(LLBC_Stream &s) const;

    /**
     * Extensional deserialize method, use to deserialize the object from stream.
     * @param[in] s - the stream object.
     * @return bool - return true if serialize success, otherwise return false.
     */
    virtual bool DeSerializeEx(LLBC_Stream &s);

    LLBC_DISABLE_ASSIGNMENT(LLBC_Object);

private:
    /**
     * Declare friend class AutoReleasePool.
     */
    friend class LLBC_AutoReleasePool;
    friend class LLBC_AutoReleasePoolStack;

    int _ref;
    int _autoRef;

    LLBC_AutoReleasePoolStack *_poolStack;
};

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_OBJECT_H__
