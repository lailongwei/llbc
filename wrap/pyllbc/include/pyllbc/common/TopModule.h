/**
 * @file    TopModule.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_TOP_MODULE_H__
#define __PYLLBC_COM_TOP_MODULE_H__

#include "pyllbc/common/LibHeader.h"
#include "pyllbc/common/PyModule.h"

/**
 * \brief The top module class encapsulation.
 */
class LLBC_HIDDEN _pyllbc_TopModule : public pyllbc_Module
{
public:
    _pyllbc_TopModule();
    virtual ~_pyllbc_TopModule();
};

/* The top module singleton macro define. */
#define pyllbc_s_TopModule LLBC_Singleton<_pyllbc_TopModule, LLBC_DummyLock>::Instance()

#endif // !__PYLLBC_COM_TOP_MODULE_H__
