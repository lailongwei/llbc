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

#include <stdexcept>
#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

class LLBC_InvalidArgException : public std::logic_error
{
public:
    explicit LLBC_InvalidArgException(const std::string &what)
        : ::std::logic_error(what) {  }
    virtual ~LLBC_InvalidArgException() LLBC_NO_EXCEPT {  }
};

class LLBC_NullPtrException : public std::logic_error
{
public:
    explicit LLBC_NullPtrException(const std::string &what)
        : ::std::logic_error(what) {  }
    virtual ~LLBC_NullPtrException() LLBC_NO_EXCEPT {  }
};

class LLBC_LengthException : public std::logic_error
{
public:
    explicit LLBC_LengthException(const std::string &what)
        : ::std::logic_error(what) {  }
    virtual ~LLBC_LengthException() LLBC_NO_EXCEPT {  }
};

class LLBC_RangeException : public std::logic_error
{
public:
    explicit LLBC_RangeException(const std::string &what)
        : ::std::logic_error(what) {  }
    virtual ~LLBC_RangeException() LLBC_NO_EXCEPT {  }
};

class LLBC_OverflowException : public std::logic_error
{
public:
    explicit LLBC_OverflowException(const std::string &what)
        : ::std::logic_error(what) {  }
    virtual ~LLBC_OverflowException() LLBC_NO_EXCEPT {  }
};

class LLBC_UnderflowException : public std::logic_error
{
public:
    explicit LLBC_UnderflowException(const std::string &what)
        : ::std::logic_error(what) {  }
    virtual ~LLBC_UnderflowException() LLBC_NO_EXCEPT {  }
};

class LLBC_IOException : public std::logic_error
{
public:
    explicit LLBC_IOException(const std::string &what)
        : std::logic_error(what) {  }
    virtual ~LLBC_IOException() LLBC_NO_EXCEPT {  }
};

__LLBC_NS_END
