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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/file/File.h"
#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/helper/JVHelper.h"

#include "llbc/core/config/Config.h"

__LLBC_NS_BEGIN

LLBC_Config::LLBC_Config()
{
    _errMsg = "OK";
}

LLBC_Config::~LLBC_Config()
{
    LLBC_STLHelper::DeleteContainer(_values);
}

int LLBC_Config::AddFile(const LLBC_String &file)
{
    if (file.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::map<LLBC_String, LLBC_JsonValue *>::iterator iter = _values.find(file);
    if (iter != _values.end())
    {
        delete iter->second;
        iter->second = NULL;
    }
    else
    {
        _values[file] = NULL;
    }

    return LLBC_OK;
}

int LLBC_Config::Initialize()
{
    std::map<LLBC_String, LLBC_JsonValue *>::iterator iter = _values.begin();
    for (; iter != _values.end(); iter ++)
    {
        LLBC_XDelete(iter->second);
    }

    iter = _values.begin();
    for (; iter != _values.end(); iter ++)
    {
        const LLBC_String &fileName = iter->first;

        LLBC_File file(fileName, LLBC_FileMode::BinaryRead);
        if (!file.IsOpened())
        {
            BuildErrMsg(fileName, LLBC_FormatLastError());
            return LLBC_FAILED;
        }

        LLBC_String content = file.ReadToEnd();

        Json::Reader reader;
        LLBC_JsonValue *value = new LLBC_JsonValue;
        if (!reader.parse(content, *value))
        {
            delete value;

            BuildErrMsg(fileName, reader.getFormatedErrorMessages().c_str());
            LLBC_SetLastError(LLBC_ERROR_INVALID);

            return LLBC_FAILED;
        }

        iter->second = value;
    }

    _errMsg = "OK";

    return LLBC_OK;
}

const LLBC_String &LLBC_Config::GetError() const
{
    return _errMsg;
}

LLBC_JsonValue LLBC_Config::GetJsonValue(const LLBC_String &file, const char *field) const
{
    if (!field)
    {
        field = ".";
    }

    std::map<LLBC_String, LLBC_JsonValue *>::const_iterator iter = _values.find(file);
    if (iter == _values.end() || !iter->second)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_JsonValue();
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    Json::Path path(field);
    return path.make(*iter->second);
}

std::vector<LLBC_JsonValue> LLBC_Config::GetJsonValueArray(const LLBC_String &file, const char *field) const
{
    LLBC_JsonValue value = GetJsonValue(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return std::vector<LLBC_JsonValue>();
    }

    std::vector<LLBC_JsonValue> values;

    LLBC_JsonValueIter jsonIter = value.begin();
    for (; jsonIter != value.end(); jsonIter ++)
    {
        values.push_back(*jsonIter);
    }

    return values;
}

std::map<int, LLBC_JsonValue> LLBC_Config::GetIntJsonMap(const LLBC_String &file, const char *field) const
{
    LLBC_JsonValue value = GetJsonValue(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return std::map<int, LLBC_JsonValue>();
    }

    std::map<int, LLBC_JsonValue> values;

    LLBC_JsonValueIter jsonIter = value.begin();
    for (; jsonIter != value.end(); jsonIter ++)
    {
        const LLBC_JsonValue &key = jsonIter.key();
        values[LLBC_Str2Int32(key.asCString())] = *jsonIter;
    }

    return values;
}

std::map<LLBC_String, LLBC_JsonValue> LLBC_Config::GetStringJsonMap(const LLBC_String &file, const char *field) const
{
    LLBC_JsonValue value = GetJsonValue(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return std::map<LLBC_String, LLBC_JsonValue>();
    }

    std::map<LLBC_String, LLBC_JsonValue> values;

    LLBC_JsonValueIter jsonIter = value.begin();
    for (; jsonIter != value.end(); jsonIter ++)
    {
        const LLBC_JsonValue &key = jsonIter.key();
        values[key.asCString()] = *jsonIter;
    }

    return values;
}

LLBC_Variant LLBC_Config::GetVariantValue(const LLBC_String &file, const char *field) const
{
    LLBC_JsonValue json = GetJsonValue(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return LLBC_Variant();
    }

    return LLBC_JVHelper::J2V(json);
}

std::vector<LLBC_Variant> LLBC_Config::GetVariantValueArray(const LLBC_String &file, const char *field) const
{
    std::vector<LLBC_JsonValue> jsons = GetJsonValueArray(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return std::vector<LLBC_Variant>();
    }

    std::vector<LLBC_Variant> variants;
    for (size_t i = 0; i < jsons.size(); i ++)
    {
        variants.push_back(LLBC_JVHelper::J2V(jsons[i]));
    }

    return variants;
}

std::map<int, LLBC_Variant> LLBC_Config::GetIntVariantMap(const LLBC_String &file, const char *field) const
{
    std::map<int, LLBC_JsonValue> jsons = GetIntJsonMap(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return std::map<int, LLBC_Variant>();
    }

    std::map<int, LLBC_Variant> variants;
    std::map<int, LLBC_JsonValue>::const_iterator iter = jsons.begin();
    for (; iter != jsons.end(); iter ++)
    {
        variants.insert(std::make_pair(
            iter->first, LLBC_JVHelper::J2V(iter->second)));
    }

    return variants;
}

std::map<LLBC_String, LLBC_Variant> LLBC_Config::GetStringVariantMap(const LLBC_String &file, const char *field) const
{
    std::map<LLBC_String, LLBC_JsonValue> jsons = GetStringJsonMap(file, field);
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    {
        return std::map<LLBC_String, LLBC_Variant>();
    }

    std::map<LLBC_String, LLBC_Variant> variants;
    std::map<LLBC_String, LLBC_JsonValue>::const_iterator iter = jsons.begin();
    for (; iter != jsons.end(); iter ++)
    {
        variants.insert(std::make_pair(
            iter->first, LLBC_JVHelper::J2V(iter->second)));
    }

    return variants;
}

void LLBC_Config::BuildErrMsg(const LLBC_String &file, const LLBC_String &err)
{
    _errMsg.format("parse file[%s] failed, err:%s", file.c_str(), err.c_str());
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
