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

#include "csllbc/common/Export.h"

#include "csllbc/core/config/_Ini.h"

namespace
{
    csllbc_Ini_IniSection *_GetAllSections(LLBC_Ini *ini, int *sectionCount)
    {
        const std::map<LLBC_String, LLBC_IniSection *> &sections = ini->GetAllSections();
        if ((*sectionCount = static_cast<int>(sections.size())) == 0)
            return nullptr;

        int sectionIdx = 0;
        csllbc_Ini_IniSection *nativeSections = LLBC_Malloc(csllbc_Ini_IniSection, sizeof(csllbc_Ini_IniSection) * *sectionCount);
        for (std::map<LLBC_String, LLBC_IniSection *>::const_iterator sectionIt = sections.begin();
             sectionIt != sections.end();
             sectionIt++)
        {
            // Get section name.
            const LLBC_String &sectionName = sectionIt->first;
            csllbc_Ini_IniSection *nativeSection = nativeSections + sectionIdx++;
            nativeSection->sectionName = 
                csllbc_PInvokeUtil::BuildCSharpStr(sectionName, &nativeSection->sectionNameLen);
            
            // Get all key-values pair in this section.
            int valueIdx = 0;
            const LLBC_IniSection *section = sectionIt->second;

            const std::map<LLBC_String, LLBC_Variant> &values = section->GetAllValues();

            size_t valueCount = values.size();
            nativeSection->count = static_cast<int>(values.size());
            if (valueCount == 0)
            {
                nativeSection->keys = nullptr;
                nativeSection->keysLen = nullptr;
                nativeSection->values = nullptr;
                nativeSection->valuesLen = nullptr;
                continue;
            }

            nativeSection->keys = LLBC_Malloc(char *, sizeof(char *) * valueCount);
            nativeSection->values = LLBC_Malloc(char *, sizeof(char *) * valueCount);
            nativeSection->keysLen = LLBC_Malloc(int, sizeof(int) * valueCount);
            nativeSection->valuesLen = LLBC_Malloc(int, sizeof(int) * valueCount);
            for (std::map<LLBC_String, LLBC_Variant>::const_iterator valueIt = values.begin();
                 valueIt != values.end();
                 valueIt++)
            {
                char *&nativeKey = nativeSection->keys[valueIdx];
                int &nativeKeyLen = nativeSection->keysLen[valueIdx];

                char *&nativeValue = nativeSection->values[valueIdx];
                int &nativeValueLen = nativeSection->valuesLen[valueIdx];

                valueIdx++;

                const LLBC_String &key = valueIt->first;
                nativeKey = csllbc_PInvokeUtil::BuildCSharpStr(key, &nativeKeyLen);
                const LLBC_Variant &value = valueIt->second;
                nativeValue = csllbc_PInvokeUtil::BuildCSharpStr(value, &nativeValueLen);
            }
        }

        return nativeSections;
    }
}

LLBC_BEGIN_C_DECL

csllbc_Ini_IniSection *csllbc_Ini_LoadFromContent(const char *content, int *sectionCount, int *errDescLen)
{
    LLBC_Ini *ini = new LLBC_Ini;
    if (ini->LoadFromContent(content) != LLBC_OK)
    {
        LLBC_String errStr = LLBC_String().format(
            "load ini failed, error: %s, detail information: %s",
            LLBC_FormatLastError(), ini->GetLoadError().c_str());

        *sectionCount = -1;
        return reinterpret_cast<csllbc_Ini_IniSection *>(csllbc_PInvokeUtil::BuildCSharpStr(errStr, errDescLen));
    }

    csllbc_Ini_IniSection *sections = _GetAllSections(ini, sectionCount);
    delete ini;

    return sections;
}

void csllbc_Ini_FreeNativeSections(csllbc_Ini_IniSection *sections, int sectionCount)
{
    if (UNLIKELY(sections == nullptr))
        return;

    for (int sectionIdx = 0; sectionIdx < sectionCount; ++sectionIdx)
    {
        csllbc_Ini_IniSection &section = sections[sectionIdx];
        free(section.sectionName);

        int valueCount = section.count;
        for (int valueIdx = 0; valueIdx < valueCount; ++valueIdx)
        {
            free(section.keys[valueIdx]);
            LLBC_XFree(section.values[valueIdx]);
        }

        if (valueCount > 0)
        {
            free(section.keys);
            free(section.keysLen);

            free(section.values);
            free(section.valuesLen);
        }
    }

    free(sections);
}

LLBC_END_C_DECL