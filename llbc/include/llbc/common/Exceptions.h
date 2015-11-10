/**
 * @file    Exceptions.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_EXCEPTIONS_H__
#define __LLBC_COM_EXCEPTIONS_H__

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

#endif // !__LLBC_COM_EXCEPTIONS_H__
