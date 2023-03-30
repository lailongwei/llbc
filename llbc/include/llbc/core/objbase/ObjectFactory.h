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

#include "llbc/core/objbase/Object.h"

__LLBC_NS_BEGIN

/**
 * \brief The object factory class encapsulation.
 *        In your project, can rewrite CreateObject() method to custom your object create way.
 */
class LLBC_EXPORT LLBC_ObjectFactory : public LLBC_Object
{
public:
    /**
     * Constructor/Destructor.
     */
    LLBC_ObjectFactory();
    virtual ~LLBC_ObjectFactory();

public:
    /**
     * Create the object.
     * @return LLBC_Object * - new object.
     */
    virtual LLBC_Object *CreateObject() const;

public:
    /**
     * Re-write clone method.
     * @return LLBC_Object * - the clone object.
     */
    virtual LLBC_Object *Clone() const;
};

__LLBC_NS_END
