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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Variant;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The Ini section class encapsulation.
 */
class LLBC_EXPORT LLBC_IniSection
{
    typedef LLBC_IniSection This;

public:
    /**
    * Default constructor.
    */
    LLBC_IniSection();

public:
    /**
     * Check has specific key config or not.
     * @param[in] key - the key.
     */
    bool IsHasKey(const LLBC_String &key) const;

    /**
     * Get specific key's value(template method).
     * @param[in] key          - the key.
     * @param[in] defaultValue - the default value, if not found.
     * @return _ValueType - the given key's value, if return defaultValue, use LLBC_GetLastError() to get error reason.
     */
    template <typename _ValueType>
    _ValueType GetValue(const LLBC_String &key, const _ValueType &defaultValue = _ValueType()) const;

    /**
    * Set value.
    * @param[in] key     - the key.
    * @param[in] value   - the value.
    * @param[in] comment - the value comment.
    * @return int - return 0 if success, otherwise return -1.
    */
    template <typename _ValueType>
    int SetValue(const LLBC_String &key, const _ValueType &value, const LLBC_String &comment = "");

    /**
    * Get specific key's comment.
    * Note:
    *       if return empty string, please use LLBC_GetLastError() to check error occurred or not.
    * @param[in] key - the key.
    * @return const LLBC_String & - the specific key/value comment.
    */
    const LLBC_String &GetComment(const LLBC_String &key) const;

public:
    /**
     * Remove specific key config.
     * @param[in] key - the key.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveValue(const LLBC_String &key);

    /**
     * Remove specific key config comment.
     * @param[in] key - the key.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveComment(const LLBC_String &key);

public:
    /**
     * Get section comment.
     * @return const LLBC_String & - the section comment.
     */
    const LLBC_String &GetSectionComment() const;
    /**
     * Set section comment.
     * @param[in] sectionComment - the section comment.
     */
    void SetSectionComment(const LLBC_String &sectionComment);

public:
    /**
     * Get all values.
     * @return const std::map<LLBC_String, LLBC_Variant> & - the all values.
     */
    const std::map<LLBC_String, LLBC_Variant> &GetAllValues() const;

    /**
     * Get all comments.
     * @return const std::map<LLBC_String, LLBC_String> & - the all comments.
     */
    const std::map<LLBC_String, LLBC_String> &GetAllComments() const;

private:
    friend class LLBC_Ini;

private:
    typedef std::map<LLBC_String, LLBC_Variant> _Values;
    typedef std::map<LLBC_String, LLBC_String> _Comments;

    LLBC_String _sectionComment;

    LLBC_Strings _keys;
    _Values _values;
    _Comments _comments;
};

/**
 * \brief The Ini sections class encapsulation(typedef).
 */
typedef std::map<LLBC_String, LLBC_IniSection *> LLBC_IniSections;

/**
* \brief The Ini file operation class encapsulation.
*/
class LLBC_EXPORT LLBC_Ini
{
    static const char SectionBegin;
    static const char SectionEnd;

    static const char KeyValueSeparator;
    static const char CommentBegin;

    static const char EscapeChar;
    static const LLBC_String BeEscapedChars;

    typedef LLBC_Ini This;

public:
    /**
    * Default constructor.
    */
    LLBC_Ini();

    /**
    * Copy constructor.
    */
    LLBC_Ini(const This &another);

    /**
    * @Destructor.
    */
    virtual ~LLBC_Ini();

public:
    /**
     * Load ini config from file.
     * @param[in] file - the file name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int LoadFromFile(const LLBC_String &file);
    /**
     * Load ini config from string content.
     * @param[in] content - the string content.
     * @return int - return 0 if success, otherwise return -1.
     */
    int LoadFromContent(const LLBC_String &content);

    /**
     * Get load error after called LoadFromContent/LoadFromFile.
     * @return const LLBC_String & - the load error.
     */
    const LLBC_String &GetLoadError() const;

    /**
     * Save ini config to file.
     * @param[in] file         - the file name.
     * @param[in] sortSections - sections sort option, default is false.
     * @param[in] sortKeys     - keys sort option, default is false.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SaveToFile(const LLBC_String &file, const LLBC_Strings &headerLines = LLBC_Strings(), bool sortSections = false, bool sortKeys = false) const;
    /**
     * Save ini config to string content.
     * @param[out] content     - the string object, ini config content will save to here.
     * @param[in] sortSections - sections sort option, default is false.
     * @param[in] sortKeys     - keys sort option, default is false.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SaveToContent(LLBC_String &content, bool sortSections = false, bool sortKeys = false) const;

public:
    /**
     * Check has section object or not.
     * @param[in] sectionName - the section name.
     * @return bool - return true if exist given name section, otherwise return false.
     */
    bool IsHasSection(const LLBC_String &sectionName) const;

    /**
     * Check has specific key in given section.
     * @param[in] sectionNme - the section name.
     * @param[in] key        - the key.
     * @return bool - return true if exist key in given section, otherwise return false.
     */
    bool IsHasKey(const LLBC_String &sectionName, const LLBC_String &key) const;

public:
    /**
     * Get section.
     * @param[in] sectionName - the section name.
     * @return LLBC_IniSection * - the section object.
     */
    LLBC_IniSection *GetSection(const LLBC_String &sectionName);

    /**
     * Get section.
     * @param[in] sectionName - the section name.
     * @return const LLBC_IniSection * - the section object.
     */
    const LLBC_IniSection *GetSection(const LLBC_String &sectionName) const;
    /**
     * Set section.
     * @param[in] sectionName - the section name.
     * @param[in] section     - the section content.
     * @param[in] merge       - merge option, if set to true and already has old section object, will perform merge operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetSection(const LLBC_String &sectionName, const LLBC_IniSection &section, bool merge = false);

public:
    /**
     * Get specific key value in given section(template method).
     * @param[in] sectionName - the section name.
     * @param[in] key         - the key.
     * @param[in] default     - default value, if not exist specific key config, return this value.
     * @return _ValueType - the config value.
     */
    template <typename _ValueType>
    _ValueType GetValue(const LLBC_String &sectionName,
                        const LLBC_String &key,
                        const _ValueType &defaultValue = _ValueType()) const;

    /**
     * Set specific key value in given section.
     * @param[in] sectionName - the section name.
     * @param[in] key         - the key.
     * @param[in] value       - the value.
     * @param[in] comment     - the comment.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _ValueType>
    int SetValue(const LLBC_String &sectionName,
                 const LLBC_String &key,
                 const _ValueType &value,
                 const LLBC_String &comment = "");

    /**
     * Get specific key comment in given section.
     * @param[in] sectionName - the section name.
     * @param[in] key         - the key.
     * @return LLBC_String - the comment, if return empty string, call LLBC_GetLastError() to determine error occurred or not.
     */
    LLBC_String GetComment(const LLBC_String &sectionName, const LLBC_String &key) const;

public:
    /**
     * Get all sections.
     * @return std::map<LLBC_String, LLBC_IniSection *> & - the all sections map.
     */
    const LLBC_IniSections &GetAllSections() const;

public:
    /**
     * assignment operation.
     */
    This &operator =(const This &another);

private:
    int ParseLine(const LLBC_String &line, size_t lineNum, LLBC_String &sectionName);
    int TryParseSectionName(const LLBC_String &content,
                            const LLBC_String &comment,
                            size_t lineNum,
                            size_t contentBeginPos,
                            LLBC_String &sectionName,
                            bool &failedContinue);
    int Split(const LLBC_String &line,
              size_t lineNum,
              size_t beginPos,
              char sep,
              LLBC_String &left,
              LLBC_String &right,
              bool requireFoundSep = false);

private:
    LLBC_String Escape(const LLBC_String &str) const;
    int UnEscape(LLBC_String &str,
                 size_t lineNum,
                 size_t beginPos,
                 size_t &unescapeTimes);

    static void EndLine(LLBC_String &str);

private:
    void Copy(const This &another);

private:
    void Err_UnSpecificSection(size_t lineNum, size_t columnNum);

    void Err_KeyEmpty(size_t lineNum, size_t columnNum);
    void Err_KeyRepeat(size_t lineNum, size_t columnNum, const LLBC_String &key);
    void Err_InvalidEscapeFormat(size_t lineNum, size_t columnNum);
    void Err_SeparatorNotFound(size_t lineNum, size_t column, char sep);

private:
    LLBC_String _errMsg;
    LLBC_Strings _sectionNames;
    LLBC_IniSections _sections;
};

__LLBC_NS_END

#include "llbc/core/config/IniInl.h"

