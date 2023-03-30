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

#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

/**
 * \brief The startup arguments class encapsulation.
 */
class LLBC_EXPORT LLBC_StartArgs
{
public:
    LLBC_StartArgs();
    virtual ~LLBC_StartArgs();

public:
    /**
     * Parse startup arguments.
     * @param[in] argc - the startup argument count.
     * @param[in] argv - the startup arguments detail.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Parse(int argc, char *argv[]);

    /**
     * Check arguments already parsed or not.
     */
    virtual bool IsParsed() const;

public:
    /**
     * Get module file path.
     * @return const LLBC_String & - the module file path.
     */
    const LLBC_String &GetModuleFilePath()const;

    /**
     * Get startup arguments count.
     * @return size_t - the startup arguments count.
     */
    size_t GetArgumentsCount() const;
    /**
     * Get all startup arguments.
     * @return std::vector<LLBC_Variant> & - the all startup arguments.
     */
    const std::vector<LLBC_Variant> &GetAllArguments() const;

    /**
     * Get startup naming arguments count.
     * @return size_t - the naming startup arguments count.
     */
    size_t GetNamingArgumentsCount() const;
    /**
     * Get all naming startup arguments.
     * @return std::map<LLBC_String, LLBC_Variant> & - the all naming startup arguments.
     */
    const std::map<LLBC_String, LLBC_Variant> &GetAllNamingArguments() const;

public:
    /**
     * Subscript operations support.
     */
    const LLBC_Variant &operator [](size_t index) const;
    const LLBC_Variant &operator [](const LLBC_String &key) const;

    LLBC_DISABLE_ASSIGNMENT(LLBC_StartArgs);

private:
    bool _parsed;

    LLBC_String _moduleFilePath;
    typedef std::vector<LLBC_Variant> _SeqArgs;
    _SeqArgs _seqArgs;
    typedef std::map<LLBC_String, LLBC_Variant> _NamingArgs;
    _NamingArgs _namingArgs;
};

__LLBC_NS_END
