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
#include "llbc/core/utils/Util_Delegate.h"
#include "llbc/core/objectpool/PoolObject.h"

__LLBC_NS_BEGIN

/**
 * \brief The event class encapsulation.
 */
class LLBC_EXPORT LLBC_Event : public LLBC_PoolObject
{
public:
    explicit LLBC_Event(int id = 0, bool dontDelAfterFire = false);
    virtual ~LLBC_Event();

public:
    /**
     * Get the event Id.
     * @return int - the event Id.
     */
    int GetId() const;

    /**
     * Set the event Id.
     * @param[in] id - the event Id.
     */
    void SetId(int id);

    /**
     * Check dont delete after fire option.
     * @return bool - the option.
     */
    bool IsDontDelAfterFire() const;

    /**
     * Set don't delete after fire option.
     * @param[in] dontDelAfterFire - the don't delete after fire option.
     */
    void SetDontDelAfterFire(bool dontDelAfterFire);

public:
    /**
     * Get integer key indexed event param.
     * @param[in] key - the integer key.
     * @return const LLBC_Variant & - the event param.
     */
    const LLBC_Variant &GetParam(int key) const;

    /**
    * Get constant string key indexed event param.
    * @param[in] key - the constant string key.
    * @return const LLBC_Variant & - the event param.
    */
    const LLBC_Variant &GetParam(const char* key) const;

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
    * Set constant string key indexed event param.
    * @param[in] key   - the param key.
    * @param[in] param - the param.
    * @return LLBC_Event & - this reference.
    */
    LLBC_Event &SetParam(const char* key, const LLBC_Variant &param);
    /**
    * Set constant string key indexed event param(template version).
    * @param[in] key   - the param key.
    * @param[in] param - the param.
    * @return LLBC_Event & - this reference.
    */
    template <typename ParamType>
    LLBC_Event &SetParam(const char* key, const ParamType &param);

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
     * Get all int key indexed params(mutable).
     * @return std::map<int, LLBC_Variant> & - the int key indexed params mutable reference.
     */
    std::map<int, LLBC_Variant> &GetMutableIntKeyParams();

    /**
     * Get all string key indexed params count.
     * @return size_t - the integer key indexed params count.
     */
    size_t GetIntKeyParamsCount() const;

    /**
    * Get all constant string key indexed params.
    * @return const std::map<LLBC_CString, LLBC_Variant> & - the constant string key indexed params reference.
    */
    const std::map<LLBC_CString, LLBC_Variant> &GetConstantStrKeyParams() const;
    /**
    * Get all constant string key indexed params(mutable).
    * @return const std::map<LLBC_CString, LLBC_Variant> & - the mutable constant string key indexed params reference.
    */
    std::map<LLBC_CString, LLBC_Variant> &GetMutableConstantStrKeyParams();

    /**
    * Get all constant string key indexed params count.
    * @return size_t - the constant string key indexed params count.
    */
    size_t GetConstantStrKeyParamsCount() const;

    /**
     * Get all string key indexed params.
     * @return const std::map<LLBC_String, LLBC_Variant> & - the constant string key indexed params reference.
     */
    const std::map<LLBC_String, LLBC_Variant> &GetStrKeyParams() const;
    /**
     * Get all string key indexed params(mutable).
     * @return std::map<LLBC_String, LLBC_Variant> & - the mutable string key indexed params reference.
     */
    std::map<LLBC_String, LLBC_Variant> &GetMutableStrKeyParams();

    /**
     * Get all string key indexed params count.
     * @return size_t - the string key indexed params count.
     */
    size_t GetStrKeyParamsCount() const;

public:
    /**
     * Clone event.
     * Note:
     *      - the clone event don't delete after handle flag always false.
     *      - the clone event extend data always nullptr.
     * @return LLBC_Event * - the clone event.
     */
    LLBC_Event *Clone() const;

public:
    /**
     * Get extend data.
     * @return void * - the extend data.
     */
    void *GetExtData() const;

    /**
     * Set extend data.
     * @param[in] extData    - the extend data.
     * @param[in] clearDeleg - the extend data clear delegate.
     */
    void SetExtData(void *extData, const LLBC_Delegate<void(void *)> &clearDeleg = nullptr);

    /**
     * Clear extend data.
     */
    void ClearExtData();

public:
    /**
     * Subscript supports.
     */
    LLBC_Variant &operator [](int key);
    LLBC_Variant &operator [](const char* key);
    LLBC_Variant &operator [](const LLBC_String &key);
    const LLBC_Variant &operator [](int key) const;
    const LLBC_Variant &operator [](const char* key) const;
    const LLBC_Variant &operator [](const LLBC_String &key) const;

public:
    /**
     * Object-Pool reflection support: clear firer object.
     */
    void Clear();

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Event);

protected:
    int _id;
    bool _dontDelAfterFire;

    typedef std::map<int, LLBC_Variant> _IntKeyParams;
    _IntKeyParams *_intKeyParams;

    typedef std::map<LLBC_CString, LLBC_Variant> _ConstantStrKeyParams;
    _ConstantStrKeyParams *_constantStrKeyParams;

    typedef std::map<LLBC_String, LLBC_Variant> _StrKeyParams;
    _StrKeyParams *_strKeyParams;

    void *_extData;
    LLBC_Delegate<void(void *)> _extDataClearDeleg;
};

/**
 * \brief The event deleg class encapsulation.
 */
class LLBC_EXPORT LLBC_EventListener
{
public:
    /**
     * Ctor & Detor.
     */
    LLBC_EventListener() = default;
    virtual ~LLBC_EventListener() = default;

public:
    /**
     * Listener invoke method.
     * @param[in] ev - the event object.
     */
    virtual void Invoke(LLBC_Event &ev) = 0;
};

__LLBC_NS_END

#include "llbc/core/event/EventInl.h"


