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

#include "llbc/common/Export.h"

#include "llbc/core/file/File.h"
#include "llbc/core/variant/Variant.h"
#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/config/Ini.h"

namespace
{
    static const LLBC_NS LLBC_Variant __nil;
    static const LLBC_NS LLBC_String __emptyStr;
}

__LLBC_NS_BEGIN

LLBC_IniSection::LLBC_IniSection()
{
}

bool LLBC_IniSection::IsHasKey(const LLBC_String &key) const
{
    return _values.find(key) != _values.end();
}

const LLBC_String &LLBC_IniSection::GetComment(const LLBC_String &key) const
{
    _Comments::const_iterator it = _comments.find(key);
    if (it == _comments.end() &&
        _values.find(key) != _values.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return __emptyStr;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return it->second;
}

int LLBC_IniSection::RemoveValue(const LLBC_String &key)
{
    _Values::iterator valIt = _values.find(key);
    if (valIt == _values.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _values.erase(valIt);
    _comments.erase(key);
    (void)std::remove(_keys.begin(), _keys.end(), key);

    return LLBC_OK;
}

int LLBC_IniSection::RemoveComment(const LLBC_String &key)
{
    _Comments::iterator it = _comments.find(key);
    if (it == _comments.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _comments.erase(it);
    return LLBC_OK;
}

const LLBC_String &LLBC_IniSection::GetSectionComment() const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return _sectionComment;
}

void LLBC_IniSection::SetSectionComment(const LLBC_String &sectionComment)
{
    _sectionComment = sectionComment;
}

const LLBC_IniSection::_Values &LLBC_IniSection::GetAllValues() const
{
    return _values;
}

const LLBC_IniSection::_Comments &LLBC_IniSection::GetAllComments() const
{
    return _comments;
}

const char LLBC_Ini::SectionBegin = '[';
const char LLBC_Ini::SectionEnd = ']';

const char LLBC_Ini::KeyValueSeparator = '=';
const char LLBC_Ini::CommentBegin = ';';

const char LLBC_Ini::EscapeChar = '\\';
const LLBC_String LLBC_Ini::BeEscapedChars = "[]=;\\\t";

LLBC_Ini::LLBC_Ini()
: _errMsg("ok")
{
}

LLBC_Ini::LLBC_Ini(const This &another)
{
    Copy(another);
}

LLBC_Ini::~LLBC_Ini()
{
    LLBC_STLHelper::DeleteContainer(_sections, true);
}

int LLBC_Ini::LoadFromFile(const LLBC_String &file)
{
    LLBC_File f(file, LLBC_FileMode::TextRead);
    if (!f.IsOpened())
        return LLBC_FAILED;

    return LoadFromContent(f.ReadToEnd());
}

int LLBC_Ini::LoadFromContent(const LLBC_String &content)
{
    LLBC_STLHelper::DeleteContainer(_sections, true);
    _sectionNames.clear();

    LLBC_String sectionName;
    const LLBC_Strings lines = content.split("\n");
    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (ParseLine(lines[i], i + 1, sectionName) != LLBC_OK)
            return LLBC_FAILED;
    }

    _errMsg = "success";
    return LLBC_OK;
}

const LLBC_String &LLBC_Ini::GetLoadError() const
{
    return _errMsg;
}

int LLBC_Ini::SaveToFile(const LLBC_String &file, const LLBC_Strings &headerLines, bool sortSections, bool sortKeys) const
{
    LLBC_File f(file, LLBC_FileMode::TextWrite);
    if (!f.IsOpened())
        return LLBC_FAILED;

    LLBC_String content;
    if (SaveToContent(content, sortSections, sortKeys) != LLBC_OK)
        return LLBC_FAILED;

    for (LLBC_Strings::const_iterator it = headerLines.begin();
         it != headerLines.end();
         ++it)
    {
        if (f.WriteLine(LLBC_String().append_format("%c %s", CommentBegin, it->c_str())) != LLBC_OK)
            return LLBC_FAILED;
    }

    if (!headerLines.empty())
        f.WriteLine("");

    return f.Write(content);
}

int LLBC_Ini::SaveToContent(LLBC_String &content, bool sortSections, bool sortKeys) const
{
    LLBC_Strings *sortedSections = nullptr;
    if (sortSections)
    {
        sortedSections = new LLBC_Strings(_sectionNames);
        std::sort(sortedSections->begin(), sortedSections->end());
    }

    const LLBC_Strings &finalSectionNames = 
        sortedSections != nullptr ? *sortedSections : _sectionNames;
    for (LLBC_Strings::const_iterator sectionIt = finalSectionNames.begin();
         sectionIt != finalSectionNames.end();
         ++sectionIt)
    {
        const LLBC_String &sectionName = *sectionIt;
        content.append_format("%c%s%c", SectionBegin, sectionName.c_str(), SectionEnd);

        const LLBC_IniSection &section = *_sections.find(sectionName)->second;
        if (!section._sectionComment.empty())
            content.append_format(" %c %s", CommentBegin, section._sectionComment.c_str());
        EndLine(content);

        LLBC_Strings sectionKeys;
        const LLBC_IniSection::_Values &sectionValues = section._values;
        for (LLBC_IniSection::_Values::const_iterator it = sectionValues.begin();
             it != section._values.end();
             ++it)
            sectionKeys.push_back(it->first);

        if (sortKeys)
            std::sort(sectionKeys.begin(), sectionKeys.end());

        for (LLBC_Strings::const_iterator keyIt = sectionKeys.begin();
             keyIt != sectionKeys.end();
             ++keyIt)
        {
            const LLBC_String &key = *keyIt;
            const LLBC_String escapedKey = Escape(key);
            const LLBC_String escapedValue = Escape(sectionValues.find(key)->second.ValueToString());
            content.append_format("%s=%s", (escapedKey).c_str(), escapedValue.c_str());

            LLBC_IniSection::_Comments::const_iterator commentIt = section._comments.find(key);
            if (commentIt != section._comments.end())
                content.append_format(" %c %s", CommentBegin, commentIt->second.c_str());
            EndLine(content);
        }

        EndLine(content);
    }

    return LLBC_OK;
}

bool LLBC_Ini::IsHasSection(const LLBC_String &sectionName) const
{
    return _sections.find(sectionName) != _sections.end();
}

bool LLBC_Ini::IsHasKey(const LLBC_String &sectionName, const LLBC_String &key) const
{
    LLBC_IniSections::const_iterator it = _sections.find(sectionName);
    if (it == _sections.end())
        return false;

    const LLBC_IniSection &section = *it->second;
    return section._values.find(key) != section._values.end();
}

LLBC_IniSection *LLBC_Ini::GetSection(const LLBC_String &sectionName)
{
    return const_cast<LLBC_IniSection *>(((const This*)this)->GetSection(sectionName));
}

const LLBC_IniSection *LLBC_Ini::GetSection(const LLBC_String &sectionName) const
{
    LLBC_IniSections::const_iterator it = _sections.find(sectionName);
    if (it == _sections.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return it->second;
}

int LLBC_Ini::SetSection(const LLBC_String &sectionName, const LLBC_IniSection &section, bool merge)
{
    if (sectionName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_IniSections::iterator it = _sections.find(sectionName);
    if (it != _sections.end())
    {
        if (merge)
        {
            LLBC_IniSection &existSection = *it->second;
            for (LLBC_IniSection::_Values::const_iterator sectionIt = section._values.begin();
                sectionIt != section._values.end();
                ++sectionIt)
            {
                const LLBC_String &key = sectionIt->first;
                LLBC_IniSection::_Comments::const_iterator commentIt = section._comments.find(key);
                existSection.SetValue(key, sectionIt->second, 
                    commentIt != section._comments.end() ? commentIt->second : "");
            }

            return LLBC_OK;
        }
        else
        {
            delete it->second;
            _sections.erase(it);
        }
    }
    else
    {
        _sectionNames.push_back(sectionName);
    }

    _sections.insert(std::make_pair(sectionName, new LLBC_IniSection(section)));
    return LLBC_OK;
}

LLBC_String LLBC_Ini::GetComment(const LLBC_String &sectionName, const LLBC_String &key) const
{
    const LLBC_IniSection *section = GetSection(sectionName);
    if (section == nullptr)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }

    return section->GetComment(key);
}

const LLBC_IniSections &LLBC_Ini::GetAllSections() const
{
    return _sections;
}

LLBC_Ini &LLBC_Ini::operator =(const This &another)
{
    if (this == &another)
        return *this;

    Copy(another);
    return *this;
}

int LLBC_Ini::ParseLine(const LLBC_String &line, size_t lineNum, LLBC_String &sectionName)
{
    if (line.empty())
        return LLBC_OK;

    // Split non-comment/comment parts.
    LLBC_String nonComment, comment;
    int ret = Split(line, lineNum, 0, CommentBegin, nonComment, comment);
    if (ret != LLBC_OK)
        return LLBC_FAILED;

    // Strip non-comment/comment.
    const size_t nonCommentLen = nonComment.length();

    nonComment.lstrip();
    const size_t leftStripLen = nonCommentLen - nonComment.length();
    nonComment.rstrip();
    if (nonComment.empty())
        return LLBC_OK;

    comment.strip();

    // Try as section name config to parse.
    bool failedContinue;
    if ((ret = TryParseSectionName(
        nonComment, comment, lineNum, leftStripLen, sectionName, failedContinue)) == LLBC_OK ||
        !failedContinue)
        return ret;

    // Split key/value parts.
    LLBC_String key, value;
    if ((ret = Split(nonComment, lineNum,
        leftStripLen, KeyValueSeparator, key, value, true)) != LLBC_OK)
        return ret;

    // Right-Strip And UnEscape key.
    if (key.rstrip().empty())
    {
        Err_KeyEmpty(lineNum, leftStripLen + 1);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }

    size_t unescapedTimes = 0;
    if ((ret = UnEscape(
        key, lineNum, leftStripLen, unescapedTimes)) != LLBC_OK)
        return ret;

    // Left-Strip And UnEscape value.
    const size_t unLeftStripValueLen = value.length();
    size_t valBeginPos = leftStripLen + key.length() + 1;

    value.lstrip();
    valBeginPos += unLeftStripValueLen - value.length();
    if ((ret = UnEscape(
        value, lineNum, valBeginPos, unescapedTimes)) != LLBC_OK)
        return ret;

    if (sectionName.empty())
    {
        Err_UnSpecificSection(lineNum, leftStripLen + 1);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }

    // Store key/value pair to IniSection object.
    LLBC_IniSection *iniSection = _sections.find(sectionName)->second;
    if (iniSection->IsHasKey(key))
    {
        Err_KeyRepeat(lineNum, leftStripLen + 1, key);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }

    return iniSection->SetValue(key, value, comment);
}

int LLBC_Ini::TryParseSectionName(const LLBC_String &content,
                                  const LLBC_String &comment,
                                  size_t lineNum,
                                  size_t contentBeginPos,
                                  LLBC_String &sectionName,
                                  bool &failedContinue)
{
    LLBC_String rawSection = content;

    // Unescape.
    size_t unescapedTimes;
    if (UnEscape(rawSection, lineNum, contentBeginPos, unescapedTimes) != LLBC_OK)
    {
        failedContinue = false;
        Err_InvalidEscapeFormat(lineNum, contentBeginPos);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);

        return LLBC_FAILED;
    }

    // Confirm format.
    if (rawSection.length() < 3 ||
        (rawSection[0] != SectionBegin || 
         rawSection[rawSection.length() - 1] != SectionEnd))
    {
        failedContinue = true;
        return LLBC_FAILED;
    }

    // Store section.
    sectionName = rawSection.substr(1, rawSection.length() - 2);
    std::map<LLBC_String, LLBC_IniSection *>::iterator it = _sections.find(sectionName);
    if (it == _sections.end())
    {
        _sections.insert(std::make_pair(sectionName, 
            new LLBC_IniSection)).first->second->SetSectionComment(comment);
        _sectionNames.push_back(sectionName);
    }
    else if (it->second->GetSectionComment().empty())
    {
        it->second->SetSectionComment(comment);
    }

    return LLBC_OK;
}

int LLBC_Ini::Split(const LLBC_String &str,
                    size_t lineNum,
                    size_t beginPos,
                    char sep,
                    LLBC_String &left,
                    LLBC_String &right,
                    bool requireFoundSep)
{
    left.clear(), right.clear();
    if (str.empty())
    {
        if (requireFoundSep)
        {
            Err_SeparatorNotFound(lineNum, beginPos + 1, sep);
            LLBC_SetLastError(LLBC_ERROR_FORMAT);

            return LLBC_FAILED;
        }

        return LLBC_OK;
    }

    bool foundSep = false;
    const size_t len = str.length();
    for (size_t pos = 0; pos < str.length(); ++pos)
    {
        const char ch = str[pos];
        if (ch == EscapeChar)
        {
            if (pos == len - 1 || 
                BeEscapedChars.find(str[pos + 1]) == LLBC_String::npos)
            {
                Err_InvalidEscapeFormat(lineNum, beginPos + 1 + pos + 1);
                LLBC_SetLastError(LLBC_ERROR_FORMAT);

                return LLBC_FAILED;
            }

            pos += 1; // Skip escaped char.
            continue;
        }

        if (ch == sep)
        {
            foundSep = true;
            left = str.substr(0, pos);
            right = str.substr(pos + 1);

            break;
        }
    }

    if (!foundSep)
    {
        if (requireFoundSep)
        {
            Err_SeparatorNotFound(lineNum, beginPos + 1 + len, sep);
            LLBC_SetLastError(LLBC_ERROR_FORMAT);

            return LLBC_FAILED;
        }

        left = str;
    }

    return LLBC_OK;
}

LLBC_String LLBC_Ini::Escape(const LLBC_String &str) const
{
    return str.escape(BeEscapedChars, EscapeChar);
}

int LLBC_Ini::UnEscape(LLBC_String &str,
                       size_t lineNum,
                       size_t beginPos,
                       size_t &unescapedTimes)
{
    unescapedTimes = 0;
    if (str.empty())
        return LLBC_OK;

    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] != EscapeChar)
            continue;

        if (i == str.length() - 1 ||
            BeEscapedChars.find(str[i + 1]) == LLBC_String::npos)
        {
            Err_InvalidEscapeFormat(lineNum, beginPos + 1 + i + 1);
            LLBC_SetLastError(LLBC_ERROR_FORMAT);

            return LLBC_FAILED;
        }

        str.erase(i, 1);
        unescapedTimes += 1;
    }

    return LLBC_OK;
}

void LLBC_Ini::Copy(const This &another)
{
    _errMsg = another._errMsg;
    _sectionNames = another._sectionNames;

    LLBC_STLHelper::DeleteContainer(_sections, true);
    for (LLBC_IniSections::const_iterator it = another._sections.begin();
         it != another._sections.end();
         ++it)
        _sections.insert(std::make_pair(it->first, new LLBC_IniSection(*it->second)));
}

void LLBC_Ini::Err_UnSpecificSection(size_t lineNum, size_t columnNum)
{
    _errMsg.format("unspecific section, at line:%ld, column:%ld", lineNum, columnNum);
}

void LLBC_Ini::Err_KeyEmpty(size_t lineNum, size_t columnNum)
{
    _errMsg.format("key empty, at line:%ld, column:%ld", lineNum, columnNum);
}

void LLBC_Ini::Err_KeyRepeat(size_t lineNum, size_t columnNum, const LLBC_String &key)
{
    _errMsg.format("key repeat, key:%s, line:%ld, column:%ld", key.c_str(), lineNum, columnNum);
}

void LLBC_Ini::Err_InvalidEscapeFormat(size_t lineNum, size_t columnNum)
{
    _errMsg.format(
        "invalid escape format(escape char['\\'] only can use to "
        "escape \";\\=\" characters), at line:%ld, column:%ld", lineNum, columnNum);
}

void LLBC_Ini::Err_SeparatorNotFound(size_t lineNum, size_t columnNum, char sep)
{
    _errMsg.format("separator[%c] not found, at line:%ld, column:%ld", sep, lineNum, columnNum);
}

__LLBC_NS_END
