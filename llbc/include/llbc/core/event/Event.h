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
#include "llbc/core/objpool/ObjPool.h"

__LLBC_NS_BEGIN

/**
 * \brief The event class encapsulation.
 */
class LLBC_EXPORT LLBC_Event : public LLBC_PoolObj
{
public:
    explicit LLBC_Event(int id = 0, bool dontDelAfterFire = false);
    virtual ~LLBC_Event() override;

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
     * Get LLBC_Variant key indexed event param.
     * @param[in] key - the LLBC_Variant key.
     * @return const LLBC_Variant & - the event param.
     */
    const LLBC_Variant &GetParam(const LLBC_Variant &key) const;

    /**
     * Get LLBC_Variant key indexed event param(template version).
     * @param[in] key - the key.
     * @return const LLBC_Variant & - the event param.
     */
    template <typename KeyType>
    const LLBC_Variant &GetParam(const KeyType &key) const;

    /**
     * Set LLBC_Variant key indexed event param.
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    LLBC_Event &SetParam(const LLBC_Variant &key, const LLBC_Variant &param);

    /**
     * Set LLBC_Variant key indexed event param(template version).
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return LLBC_Event & - this reference.
     */
    template <typename KeyType, typename ParamType>
    LLBC_Event &SetParam(const KeyType &key, const ParamType &param);

public:
    /**
     * Get all variant key indexed params.
     * @return const std::map<LLBC_Variant, LLBC_Variant> & - the variant key indexed params const reference.
     */
    const std::map<LLBC_Variant, LLBC_Variant> &GetParams() const;

    /**
     * Get all variant key indexed params(mutable).
     * @return std::map<LLBC_Variant, LLBC_Variant> & - the variant key indexed params mutable reference.
     */
    std::map<LLBC_Variant, LLBC_Variant> &GetMutableParams();

    /**
     * Clone event.
     * Note:
     *      - the clone event don't delete after handle flag always false.
     *      - the clone event extend data always nullptr.
     * @return LLBC_Event * - the clone event.
     */
    LLBC_Event *Clone() const;

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
    LLBC_Variant &operator[](const LLBC_Variant &key);
    const LLBC_Variant &operator[](const LLBC_Variant &key) const;

    template <typename KeyType>
    LLBC_Variant &operator[](const KeyType &key);
    template <typename KeyType>
    const LLBC_Variant &operator[](const KeyType &key) const;

public:
    /**
     * Object-Pool reflection support: Reuse Event object.
     */
    virtual void Reuse();

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Event);

protected:
    int _id;
    bool _dontDelAfterFire;

    std::map<LLBC_Variant, LLBC_Variant> _params;

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
     * Ctor & Dtor.
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
