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
 * Pre-Declare: LLBC_File.
 */
class LLBC_File;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The properties format config load&store support.
 *
 */
class LLBC_EXPORT LLBC_Properties
{
public:
    /**
     * Load properties from file.
     * @param[in] filePath   - the properties file path.
     * @param[in] properties - the properties, if failed, will set to Nil.
     * @param[in] errMsg     - the error message, if failed, will be set to this param, otherwise set to 'Success'.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int LoadFromFile(const LLBC_String &filePath,
                            LLBC_Variant &properties,
                            LLBC_String *errMsg = nullptr);

    /**
     * Load properties from string content.
     * @param[in] str        - the properties string content.
     * @param[in] properties - the properties, if failed, will set to Nil.
     * @param[in] errMsg     - the error message, if failed, will be set to this param.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int LoadFromString(const LLBC_String &str,
                              LLBC_Variant &properties,
                              LLBC_String *errMsg = nullptr);

    /**
     * Save properties to file.
     * @param[in] properties - the property.
     * @param[in] filePath   - the properties file path.
     * @param[in] errMsg     - the error message, if failed, will be set to this param.
     * @return int - return 0 if success, otherwise return -1. 
     */
    static int SaveToFile(const LLBC_Variant &properties,
                          const LLBC_String &filePath,
                          LLBC_String *errMsg = nullptr);

    /**
     * Write property to string content.
     * @param[in]  properties - the properties.
     * @param[out] str        - the properties content.
     * @param[in]  errMsg     - the error message, if failed, will be set to this param.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int SaveToString(const LLBC_Variant &properties,
                            LLBC_String &str,
                            LLBC_String *errMsg = nullptr);

private:
    /**
     * Parse property line.
     */
    static int ParseLine(int lineNo,
                         const LLBC_String &line,
                         LLBC_Strings &keyItems,
                         LLBC_String &value,
                         LLBC_String *errMsg);

    /**
     * Write property line.
     */
    static int SaveLine(const LLBC_String key,
                        const LLBC_Variant &property,
                        LLBC_String &str,
                        LLBC_String *errMsg);

    /**
     * Check property key item.
     */
    static bool CheckKeyItem(const LLBC_String &keyItem);

    /**
     * Check property key items.
     */
    static bool CheckKeyItems(const LLBC_Strings &keyItems);

    /**
     * Escape property value.
     */
    static void EscapeValue(const LLBC_String &rawValue, LLBC_String &escapedValue);
    /**
     * Unescape property value.
     */
    static int UnescapeValue(int lineNo,
                             const LLBC_String &escapedValue,
                             LLBC_String &rawValue,
                             LLBC_String *errMsg);
};

__LLBC_NS_END
