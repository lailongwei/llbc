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
 * Pre-declare some classes.
 */
class LLBC_IHolder;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The entity behavior value structure encapsulation.
 */
struct LLBC_BehaviorValue
{
    // Behavior Id.
    int id;
    // Behavior name.
    LLBC_String name;

    // Value holder.
    LLBC_IHolder *holder;
};

/**
 * \brief The entity class encapsulation.
 */
class LLBC_EXPORT LLBC_Entity
{
public:
    LLBC_Entity();
    ~LLBC_Entity();

public:
    /**
     * Get entity Id.
     * @return int - entity Id.
     */
    int GetId() const;

    /**
     * Set entity Id.
     * @param[in] id - entity Id.
     */
    void SetId(int id);

    /**
     * Get entity name.
     * @return const LLBC_String & - the entity name.
     */
    const LLBC_String &GetName() const;

    /**
     * Set entity name.
     * @param[in] name - entity name.
     */
    void SetName(const LLBC_String &name);

public:
    /**
     * Add property to entity.
     * @param[in] name  - property name.
     * @param[in] value - property value.
     */
    int AddProperty(const LLBC_String &name, const LLBC_Variant &value);

    /**
     * Check specific name's property exist or not.
     * @param[in] name - property name.
     * @return bool - return true if given name's property exist, otherwise return false.
     */
    bool IsExistProperty(const LLBC_String &name) const;

    /**
     * Get property from entity.
     * @param[in] name - property name.
     * @return LLBC_Variant & - property value.
     */
    LLBC_Variant &GetProperty(const LLBC_String &name);

    /**
     * Get property from entity(const version).
     * @param[in] name - property name.
     * @return const LLBC_Variant & - property value(const reference).
     */
    const LLBC_Variant &GetProperty(const LLBC_String &name) const;

    /**
     * Remove property.
     * @param[in] name - property name.
     * @return int - return 0 if remove successed, otherwise return -1.
     */
    int RemmoveProperty(const LLBC_String &name);

    /**
     * Remove all properties.
     */
    void RemoveAllProperties();

public:
    /**
     * Add behavior to entity.
     * @param[in] id    - behavior Id(unique).
     * @param[in] value - behavior.
     * @param[in] name  - behavior name.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename BehaviorType>
    int AddBehavior(int id, BehaviorType *value, const LLBC_String &name = "");

    /**
     * Check given Id's behavior exist or not.
     * @param[in] id - property Id.
     * @return bool - return true if exist, otherwise return false.
     */
    bool IsExistBehavior(int id) const;

    /**
     * Check given name's behavior exist or not.
     * @param[in] name - property name.
     * @return bool - return true if exist, otherwise return false.
     */
    bool IsExistBehavior(const LLBC_String &name) const;

    /**
     * Get behavior by Id.
     * @param[in] id - behavior Id.
      *@return BehaviorType * - behavior.
      */
    template <typename BehaviorType>
    BehaviorType *GetBehavior(int id);

    /**
     * Get behavior by Id(const version).
     * @param[in] id - behavior Id.
     * @return const BehaviorType * - behavior.
     */
    template <typename BehaviorType>
    const BehaviorType *GetBehavior(int id) const;

    /**
     * Get behavior by name.
     * @param[in] name - behavior name.
     * @return BehaviorType * - behavior.
     */
    template <typename BehaviorType>
    BehaviorType *GetBehavior(const LLBC_String &name);

    /**
     * Get behavior by name(const version).
     * @param[in] name - behavior name.
     * @return const BehaviorType * - behavior.
     */
    template <typename BehaviorType>
    const BehaviorType *GetBehavior(const LLBC_String &name) const;

    /**
     * Get all behaviors.
     * @return const std::map<int, LLBC_BehaviorValue> & - behaviors map.
     */
    const std::map<int, LLBC_BehaviorValue> &GetIdBehaviorMap() const;

    /**
     * Get all behaviors.
     * @return const std::map<LLBC_String, LLBC_BehaviorValue> & - behaviors map.
     */
    const std::map<LLBC_String, LLBC_BehaviorValue> &GetNameBehaviorMap() const;

    /**
     * Remove behavior by Id.
     * @param[in] id - behavior Id.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveBehavior(int id);

    /**
     * Remove behavior by name.
     * @param[in] name - behavior name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveBehavior(const LLBC_String &name);

    /**
     * Remove all behaviors.
     */
    void RemoveAllBehaviors();

public:
    int _id;
    LLBC_String _name;

    std::map<LLBC_String, LLBC_Variant> _properties;

    std::map<int, LLBC_BehaviorValue> _behaviors;
    std::map<LLBC_String, LLBC_BehaviorValue> _behaviors2;
};

__LLBC_NS_END

#include "llbc/core/entity/EntityInl.h"


