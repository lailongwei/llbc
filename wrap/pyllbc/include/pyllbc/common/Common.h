/**
 * @file    Common.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2104/07/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_COMMON_H__
#define __PYLLBC_COM_COMMON_H__

#include "pyllbc/common/LibHeader.h"
#include "pyllbc/common/Config.h"
#include "pyllbc/common/Macro.h"
#include "pyllbc/common/Errors.h"
#include "pyllbc/common/PyStream.h"
#include "pyllbc/common/MethodTable.h"
#include "pyllbc/common/PyModule.h"
#include "pyllbc/common/ObjUtil.h"
#include "pyllbc/common/ObjAttrOptr.h"
#include "pyllbc/common/PyTypeDetector.h"

/**
 * Initialize pyllbc common module.
 */
LLBC_HIDDEN void pyllbc_InitCommon();

/**
 * Destroy pytllbc common module.
 */
LLBC_HIDDEN void pyllbc_DestroyCommon();

/**
 * Add common module all methods to python.
 */
LLBC_HIDDEN void pyllbc_AddCommonMethods();

/**
 * Add common module all objects to python.
 */
LLBC_HIDDEN void pyllbc_AddCommonObjs();

#endif // !__PYLLBC_COM_COMMON_H__

