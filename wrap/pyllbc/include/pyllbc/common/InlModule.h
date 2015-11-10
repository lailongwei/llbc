/**
 * @file    InlModule.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_INL_MODULE_H__
#define __PYLLBC_COM_INL_MODULE_H__

#include "pyllbc/common/LibHeader.h"
#include "pyllbc/common/PyModule.h"

/**
 * \breif The internal module class encapsulation.
 */
class _pyllbc_InlModule : public pyllbc_Module
{
public:
    _pyllbc_InlModule();
    virtual ~_pyllbc_InlModule();
};

/* The internal module singleton class encapsulation. */
#define pyllbc_s_InlModule LLBC_Singleton<_pyllbc_InlModule, LLBC_DummyLock>::Instance()

#endif // !__PYLLBC_COM_INL_MODULE_H__
