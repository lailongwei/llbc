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

#ifndef __LLBC_CORE_EVENT_EVENT_H__
#define __LLBC_CORE_EVENT_EVENT_H__

#include "llbc/common/Common.h"
#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

/**
 * \brief The event class encapsulation.
 */
class LLBC_EXPORT LLBC_Event
{
public:
    LLBC_Event(int id = 0, bool dontDelAfterFire = false);
    virtual ~LLBC_Event();

public:
    /**
     * Get the event Id.
     * @return int - the event Id.
     */
    int GetId() const;

public:
    /**
     * Add sequential event param.
     * @param[in] param - the event param.
     * @return LLBC_Event & - this reference.
     */
    LLBC_Event &AddParam(const LLBC_Variant &param);
    /**
     * Add sequential event param(template version).
     * @param[in] param - the event param.
     * @return LLBC_Event & - this reference.
     */
    template <typename ParamType>
    LLBC_Event &AddParam(const ParamType &param);

    /**
     * Add naming event param.
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    LLBC_Event &AddParam(const LLBC_String &key, const LLBC_Variant &param);
    /**
     * Add naming event param(template version).
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    template <typename ParamType>
    LLBC_Event &AddParam(const LLBC_String &key, const ParamType &param);

public:
    /**
     * Get all sequential params.
     * @return const std::vector<LLBC_Variant> & - the sequential params const reference.
     */
    const std::vector<LLBC_Variant> &GetSequentialParams() const;

    /**
     * Get sequential params count.
     * @return size_t - the sequential params count.
     */
    size_t GetSequentialParamsCount() const;

    /**
     * Get all naming params.
     * @return const std::map<LLBC_String, LLBC_Variant> & - the naming params const reference.
     */
    const std::map<LLBC_String, LLBC_Variant> &GetNamingParams() const;

    /**
     * Get naming params count.
     * @return size_t - the naming params count.
     */
    size_t GetNamingParamsCount() const;

    /**
     * Check dont delete after fire option.
     * @return bool - the option.
     */
    bool IsDontDelAfterFire() const;

public:
    /**
     * Subscript supports.
     */
    const LLBC_Variant &operator [](size_t index) const;
    const LLBC_Variant &operator [](const LLBC_String &key) const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Event);

protected:
    int _id;
    bool _dontDelAfterFire;

    typedef std::vector<LLBC_Variant> _SeqParams;
    _SeqParams *_seqParams;

    typedef std::map<LLBC_String, LLBC_Variant> _NamingParams;
    _NamingParams *_namingParams;
};

__LLBC_NS_END

#include "llbc/core/event/EventImpl.h"

#endif // !__LLBC_CORE_EVENT_EVENT_H__
