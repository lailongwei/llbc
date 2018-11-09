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

    /**
     * Check dont delete after fire option.
     * @return bool - the option.
     */
    bool IsDontDelAfterFire() const;

    /**
     * Set dont delete after fire option
     * @return void
     */
    void SetDontDelAfterFire(bool isDontDelAfterFire);

public:
    /**
     * Get integer key indexed event param.
     * @param[in] key - the integer key.
     * @return const LLBC_Variant & - the event param.
     */
    const LLBC_Variant &GetParam(int key) const;

    /**
     * Get string key indexed event param.
     * @param[in] key - the string key.
     * @return const LLBC_Variant & - the event param.
     */
    const LLBC_Variant &GetParam(const LLBC_String &key) const;

    /**
     * Set integer key indexed event param.
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    LLBC_Event &SetParam(int key, const LLBC_Variant &param);
    /**
     * Set integer key indexed event param(template version).
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    template <typename ParamType>
    LLBC_Event &SetParam(int key, const ParamType &param);

    /**
     * Set string key indexed event param.
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    LLBC_Event &SetParam(const LLBC_String &key, const LLBC_Variant &param);
    /**
     * Set string key indexed event param(template version).
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    template <typename ParamType>
    LLBC_Event &SetParam(const LLBC_String &key, const ParamType &param);

public:
    /**
     * Get all int key indexed params.
     * @return const std::map<int, LLBC_Variant> & - the int key indexed params const reference.
     */
    const std::map<int, LLBC_Variant> &GetIntKeyParams() const;

    /**
     * Get all string key indexed params count.
     * @return size_t - the integer key indexed params count.
     */
    size_t GetIntKeyParamsCount() const;

    /**
     * Get all string key indexed params.
     * @return const std::map<LLBC_String, LLBC_Variant> & - the string key indexed params const reference.
     */
    const std::map<LLBC_String, LLBC_Variant> &GetStrKeyParams() const;

    /**
     * Get all string key indexed params count.
     * @return size_t - the string key indexed params count.
     */
    size_t GetStrKeyParamsCount() const;

public:
    /**
     * Subscript supports.
     */
    LLBC_Variant &operator [](int key);
    LLBC_Variant &operator [](const LLBC_String &key);
    const LLBC_Variant &operator [](int key) const;
    const LLBC_Variant &operator [](const LLBC_String &key) const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Event);

protected:
    int _id;
    bool _dontDelAfterFire;

    typedef std::map<int, LLBC_Variant> _IntKeyParams;
    _IntKeyParams *_intKeyParams;

    typedef std::map<LLBC_String, LLBC_Variant> _StrKeyParams;
    _StrKeyParams *_strKeyParams;
};

__LLBC_NS_END

#include "llbc/core/event/EventImpl.h"

#endif // !__LLBC_CORE_EVENT_EVENT_H__
