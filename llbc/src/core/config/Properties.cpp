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
#include "llbc/core/config/Properties.h"

namespace
{

/**
 * \brief The property separator characters define.
 */
class __LLBC_PropertySeps
{
public:
    static constexpr char KeyValueSep = '='; // Property key/value separator.
    static constexpr char KeyItemSep = '.'; // Property key item separator.
    static constexpr char CommonBegSep = '#'; // Property comment separator.
    static constexpr char EscapeChar = '\\'; // Escape char, Used for escape: '#', '\ ', '<spaces>'
};

}

__LLBC_NS_BEGIN

int LLBC_Properties::LoadFromFile(const LLBC_String &filePath,
                                  LLBC_Variant &properties,
                                  LLBC_String *errMsg)
{
    LLBC_File file;
    if (file.Open(filePath, LLBC_FileMode::TextRead) != LLBC_OK)
    {
        LLBC_DoIf(errMsg,
                  errMsg->format("Open properties file '%s' failed, error:%s",
                  filePath.c_str(), LLBC_FormatLastError()))
        return LLBC_FAILED;
    }

    const auto str = file.ReadToEnd();
    if (str.empty() && LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
        return LLBC_FAILED;

    file.Close();
    return LoadFromString(str, properties, errMsg);
}

int LLBC_Properties::LoadFromString(const LLBC_String &str,
                                    LLBC_Variant &properties,
                                    LLBC_String *errMsg)
{
    LLBC_String value;
    LLBC_Strings keyItems;

    // Foreach parse property lines.
    properties.BecomeDict();
    const auto lines = str.split("\n", -1, true);
    for (size_t i = 0; i < lines.size(); ++i)
    {
        // Parse line.
        value.clear();
        keyItems.clear();
        if (ParseLine(static_cast<int>(i) + 1, lines[i], keyItems, value, errMsg) != LLBC_OK)
            return LLBC_FAILED;

        // If keyItems is empty, continue.
        if (keyItems.empty())
            continue;

        // Fill property line content variant object.
        LLBC_Variant *prevProperty = &properties;
        for (size_t i = 0; i < keyItems.size() - 1; ++i)
        {
            prevProperty = &(*prevProperty)[keyItems[i]];
            prevProperty->BecomeDict();
        }

        (*prevProperty)[keyItems[keyItems.size() - 1]] = value;
    }

    LLBC_DoIf(errMsg, errMsg->assign("Success"));

    return LLBC_OK;
}

int LLBC_Properties::SaveToFile(const LLBC_Variant &properties,
                                LLBC_String &filePath,
                                LLBC_String *errMsg)
{
    LLBC_String propContent;
    if (SaveToString(properties, propContent, errMsg) != LLBC_OK)
        return LLBC_FAILED;

    LLBC_File file;
    if (file.Open(filePath, LLBC_FileMode::TextWrite) != LLBC_OK)
    {
        LLBC_DoIf(errMsg,
                  errMsg->format("Open file '%s' failed, error:%s",
                                 filePath.c_str(), LLBC_FormatLastError()));
        return LLBC_FAILED;
    }

    if (file.Write(propContent) != LLBC_OK)
    {
        LLBC_DoIf(errMsg,
                  errMsg->format("Write property to file '%s' failed, error:%s",
                                 filePath.c_str(), LLBC_FormatLastError()));
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_Properties::SaveToString(const LLBC_Variant &properties,
                                  LLBC_String &content,
                                  LLBC_String *errMsg)
{
    // Check properties value.
    if (!properties.IsDict())
    {
        LLBC_DoIf(errMsg,
                  errMsg->format("Properties must be a dictionary, property type:'%s'",
                                 properties.TypeToString().c_str()));
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }

    if (SaveLine("", properties, content, errMsg) != LLBC_OK)
        return LLBC_FAILED;

    LLBC_DoIf(errMsg, errMsg->assign("Success"));
    
    return LLBC_OK;
}

int LLBC_Properties::ParseLine(int lineNo,
                               const LLBC_String &line,
                               LLBC_Strings &keyItems,
                               LLBC_String &value,
                               LLBC_String *errMsg)
{
    // Skip comment line/empty line.
    auto lstrippedLine = line.lstrip();
    if (lstrippedLine.empty() ||
        lstrippedLine[0] == __LLBC_PropertySeps::CommonBegSep)
        return LLBC_OK;

    // Split key & value, format a.b.c = xxxx
    const auto keyValuePair = lstrippedLine.split(__LLBC_PropertySeps::KeyValueSep, 1);
    if (keyValuePair.size() != 2)
    {
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        LLBC_DoIf(errMsg,
                  errMsg->format("#%d: No property key/value separator('%c') found",
                                 lineNo, __LLBC_PropertySeps::KeyValueSep));
        return LLBC_FAILED;
    }

    // Split key items.
    const auto key = keyValuePair[0].strip();
    keyItems = key.split(__LLBC_PropertySeps::KeyItemSep, -1, false);
    if (!CheckKeyItems(keyItems))
    {
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        LLBC_DoIf(errMsg, errMsg->format("#%d: Property key invalid, key:'%s'", lineNo, key.c_str()));
        return LLBC_FAILED;
    }

    // Get value.
    size_t valueEndPos = LLBC_String::npos;
    const auto &valueComment = keyValuePair[1].lstrip();
    for (size_t i = 0; i < valueComment.size(); ++i)
    {
        // If ch is not a common begin separator, continue.
        if (valueComment[i] != __LLBC_PropertySeps::CommonBegSep)
            continue;

        // Count consecutive escape char count.
        size_t escapeCharCnt = 0;
        for (int j = static_cast<int>(i - 1); j >= 0; --j)
        {
            if (valueComment[j] != __LLBC_PropertySeps::EscapeChar)
                break;
            ++escapeCharCnt;
        }

        // If consecutive escape char count is even, mark as value end.
        if (escapeCharCnt % 2 == 0)
        {
            valueEndPos = i;
            break;
        }
    }

    const auto rawValue =
        valueEndPos != LLBC_String::npos ? valueComment.substr(0, valueEndPos) : valueComment;

    // Right-Strip value.
    size_t rstripPos = rawValue.size();
    for (int i = static_cast<int>(rawValue.size()) - 1; i >= 0; --i)
    {
        // If ch is not a space, break.
        if (!LLBC_IsSpace(rawValue[i]))
            break;

        // Count consecutive escape char count.
        size_t escapeCharCnt = 0;
        for (int j = i - 1; j >= 0; --j)
        {
            if (rawValue[j] != __LLBC_PropertySeps::EscapeChar)
                break;
            ++escapeCharCnt;
        }

        // If consecutive escape char count is even, mark as value end.
        if (escapeCharCnt % 2 == 0)
        {
            rstripPos = i;
            continue;
        }
    }

    // Unescape value(process escape).
    if (UnescapeValue(lineNo,
                      rstripPos != rawValue.size() ? rawValue.substr(0, rstripPos) : rawValue,
                      value,
                      errMsg) != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

int LLBC_Properties::SaveLine(const LLBC_String key,
                              const LLBC_Variant &value,
                              LLBC_String &propStr,
                              LLBC_String *errMsg)
{
    // Real write property line lambda.
    auto writePropLine = [&]() {
        LLBC_String escapedValue;
        EscapeValue(value.ValueToString(), escapedValue);

        LLBC_String propLine;
        propStr.append_format("%s = %s\n", key.c_str(), escapedValue.c_str());

        return LLBC_OK;
    };

    // If propValue is a dict, write dict items.
    if (value.IsDict())
    {
        if (!value)
            return key.empty() ? LLBC_OK : writePropLine();

        for (auto it = value.DictBegin(); it != value.DictEnd(); ++it)
        {
            const auto keyItem = it->first.AsStr();
            const auto newPropKey =
                key.empty() ? keyItem : key + __LLBC_PropertySeps::KeyItemSep + keyItem;
            if (!CheckKeyItem(keyItem))
            {
                LLBC_DoIf(errMsg, errMsg->format("Property key invalid, key:'%s'", newPropKey.c_str()));
                LLBC_SetLastError(LLBC_ERROR_FORMAT);
                return LLBC_FAILED;
            }

            if (SaveLine(newPropKey, it->second, propStr, errMsg) != LLBC_OK)
                return LLBC_FAILED;
        }

        return LLBC_OK;
    }

    return writePropLine();
}

bool LLBC_Properties::CheckKeyItem(const LLBC_String &keyItem)
{
    if (keyItem.empty())
        return false;

    for (size_t i = 0; i < keyItem.size(); ++i)
    {
        if (!(keyItem[i] == '_' ||
              std::isalpha(keyItem[i]) ||
              (i != 0 && std::isdigit(keyItem[i]))))
        {
            return false;
        }
    }

    return true;
}

bool LLBC_Properties::CheckKeyItems(const LLBC_Strings &keyItems)
{
    if (keyItems.empty())
        return false;

    for (auto &keyItem : keyItems)
    {
        if (!CheckKeyItem(keyItem))
            return false;
    }

    return true;
}

void LLBC_Properties::EscapeValue(const LLBC_String &rawValue, LLBC_String &escapedValue)
{
    escapedValue.clear();
    for (auto &ch : rawValue)
    {
        if (LLBC_IsSpace(ch) ||
            ch == __LLBC_PropertySeps::EscapeChar ||
            ch == __LLBC_PropertySeps::CommonBegSep)
            escapedValue.append(1, __LLBC_PropertySeps::EscapeChar);
        escapedValue.append(1, ch);
    }
}

int LLBC_Properties::UnescapeValue(int lineNo,
                                   const LLBC_String &escapedValue,
                                   LLBC_String &rawValue,
                                   LLBC_String *errMsg)
{
    rawValue.clear();
    for (size_t i = 0; i < escapedValue.size(); ++i)
    {
        if (escapedValue[i] == __LLBC_PropertySeps::EscapeChar)
        {
            if (i + 1 == escapedValue.size())
            {
                LLBC_DoIf(errMsg,
                          errMsg->format("#%d: Found escape char('\\') at value end, value:'%s'",
                                         lineNo, escapedValue.c_str()));
                LLBC_SetLastError(LLBC_ERROR_FORMAT);
                return LLBC_FAILED;
            }

            const auto &nextCh = escapedValue[i + 1];
            if (!LLBC_IsSpace(nextCh) &&
                nextCh != __LLBC_PropertySeps::EscapeChar &&
                nextCh != __LLBC_PropertySeps::CommonBegSep)
            {
                LLBC_DoIf(errMsg,
                          errMsg->format("#%d: Found invalid escape char('\\%c') at value, value:'%s'",
                                         lineNo, nextCh, escapedValue.c_str()));
                LLBC_SetLastError(LLBC_ERROR_FORMAT);
                return LLBC_FAILED;
            }

            ++i;
            rawValue.append(1, nextCh);
        }
        else
        {
            rawValue.append(1, escapedValue[i]);
        }
    }

    return LLBC_OK;
}

__LLBC_NS_END
