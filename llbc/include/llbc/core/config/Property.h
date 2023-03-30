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
 * \brief The property encapsulation.
 */

class LLBC_EXPORT LLBC_Property
{
public:
    static const char NameSeparator;
    static const char NameValueSeparator;
    static const char CommentBeg;

    static const char EscapeChar;
    static const char * const EscapeChars;

    typedef LLBC_Property This;

public:
    /**
     * Properties type typedef.
     */
    typedef std::map<LLBC_String, LLBC_Property *> Properties;

public:
    /**
     * Constructor / Destructor.
     */
    LLBC_Property(const This &another);
    explicit LLBC_Property(LLBC_Property *parent = nullptr);

    virtual ~LLBC_Property();

public:
    /**
     * Load properties data from file.
     * @param[in] file - the file name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int LoadFromFile(const LLBC_String &file);

    /**
     * Load properties data from string content.
     * @param[in] content - properties string content.
     * @return int - return 0 if success, otherwise return -1.
     */
    int LoadFromContent(const LLBC_String &content);

    /**
     * @Save properties to file.
     * @param[in] file - the file name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SaveToFile(const LLBC_String &file) const;

    /**
     * Save properties to string content.
     * @param[out] content - the string content.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SaveToContent(LLBC_String &content) const;

    /**
     * Get the error describe, if call LoadFromFile/LoadFromContent failed, 
     * call this method to fetch the verbose error describe.
     * @return const LLBC_String & - the error describe.
     */
    const LLBC_String &GetLoadErrorDesc() const;

public:
    /**
     * Get the property's parent property.
     * @return LLBC_Property * - the parent property.
     */
    const LLBC_Property *GetParent() const;

    /**
     * Get the property name.
     * @return LLBC_String - the property name.
     */
    LLBC_String GetName() const;

    /**
     * Get the property count.
     * @return size_t - the properties count.
     */
    size_t GetPropertiesCount() const;

    /**
     * Get the property names.
     * @param[in] nest - nest flag.
     * @return LLBC_Strings - the property names.
     */
    LLBC_Strings GetPropertyNames(bool nest = false) const;

public:
    /**
     * Get property value.
     * @param[in] name - the property name, if empty, will return self value.
     * @param[in] dft  - the default value, if this property has not value.
     * @return LLBC_Variant - the property value, if has no value, return default value, 
     *                        and set last error to LLBC_ERROR_NOT_FOUND, otherwise set 
     *                        last error to LLBC_ERROR_SUCCESS.
     */
    LLBC_Variant GetValue(const LLBC_String &name = LLBC_String(),
                          const LLBC_Variant &dft = LLBC_Variant()) const;

    /**
     * Set property value.
     * @param[in] name     - set property name, if empty, will set to self.
     * @param[in] value    - the property value.
     * @param[in] comments - the comments.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _ValueType>
        int SetValue(const LLBC_String &name, const _ValueType &value, const LLBC_String &comments = LLBC_String());

public:
    /**
     * Check this property has specific name's sub property or not.
     * @param[in] name - the property name, can be a full name by dot separate.
     */
    bool HasProperty(const LLBC_String &name) const;

    /**
     * Get property by name.
     * @param[in] name - the property name, can be a full name by dot separate.
     * @return const LLBC_Property * - the sub property.
     */
    const LLBC_Property *GetProperty(const LLBC_String &name) const;

    /**
     * Get all property sub-properties.
     * @return const Properties - the all sub properties.
     */
    const Properties &GetAllProperties() const;

    /**
     * Remove the sub property.
     * @param[in] name      - the property name.
     * @param[in] removeAll - remove all properties or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveProperty(const LLBC_String &name, bool removeAll = false);

    /**
     * Return the property's all sub properties.
     */
    void RemoveAllProperties();

public:
    /**
     * Get the property comments
     * @param[in] name - the property name, default is empty string.
     * @return LLBC_String - the comments string.
     */
    LLBC_String GetComments(const LLBC_String &name = LLBC_String()) const;
    
    /**
     * Set the property's or sub property's comments.
     * @param[in] name     - will set to sub property's name, if empty, will set to self.
     * @param[in] comments - the property comments, the comments can be set only when 
     *                         this property has not sub properties.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetComments(const LLBC_String &name, const LLBC_String &comments);

public:
    This &operator =(const This &another);

private:
    void Cleanup();
    void CleanupValue();
    void CleanupComments();

    void ExpandProperties(const LLBC_String &name);

    bool CheckName(const LLBC_String &name, bool allowSeparator = true) const;

    LLBC_Strings NormalizeContent(const LLBC_String &content);


    int ParseLine(const LLBC_String &line, int lineNo);

    LLBC_String SerializeValue() const;

    void CollectValueProperties(std::vector<const This *> &properties) const;

private:
    LLBC_Property *_parent;

    LLBC_Variant *_value;
    Properties *_properties;

    LLBC_String _comments;

private:
    LLBC_String _errMsg;
};

__LLBC_NS_END

#include "llbc/core/config/PropertyInl.h"


