/**
 * @file    _Ini.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/06
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_CORE_CONFIG__INI_H__
#define __CSLLBC_CORE_CONFIG__INI_H__

#include "csllbc/common/Common.h"

struct CSLLBC_EXPORT csllbc_Ini_IniSection
{
    char *sectionName;
    int sectionNameLen;

    char **keys;
    int *keysLen;
    char **values;
    int *valuesLen;

    int count;
};

LLBC_EXTERN_C CSLLBC_EXPORT csllbc_Ini_IniSection *csllbc_Ini_LoadFromContent(const char *content,int *sectionCount, int *errDescLen);
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Ini_FreeNativeSections(csllbc_Ini_IniSection *sections, int sectionCount);

#endif // !__CSLLBC_CORE_CONFIG__INI_H__
