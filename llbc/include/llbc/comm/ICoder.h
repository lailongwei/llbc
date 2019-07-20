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

#ifndef __LLBC_COMM_CODER_H__
#define __LLBC_COMM_CODER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Packet;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The packet coder interface class encapsulation.
 */
class LLBC_ICoder
{
public:
    virtual ~LLBC_ICoder() {  }

public:
    /**
     * Encode pure virtual function, implement it to use encode packet data.
     */
    virtual bool Encode(LLBC_Packet &packet) = 0;

    /**
     * Decode pure virtual function, implement it to use decode packet data.
     */
    virtual bool Decode(LLBC_Packet &packet) = 0;
};

/**
 * \brief The packet coder factory interface class encapsulation.
 */
class LLBC_ICoderFactory
{
public:
    virtual ~LLBC_ICoderFactory() {  }

public:
    /**
     * Create coder.
     * @return LLBC_ICoder * - coder.
     */
    virtual LLBC_ICoder *Create() const = 0;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_CODER_H__
