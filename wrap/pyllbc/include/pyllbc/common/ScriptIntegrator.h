/**
 * @file    ScriptIntegrator.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_SCRIPT_INTEGRATOR_H__
#define __PYLLBC_COM_SCRIPT_INTEGRATOR_H__

#include "pyllbc/common/LibHeader.h"

/**
 * \brief The python script integrator class encapsulation.
 *        use to integrate python script to pyllbc module.
 */
class LLBC_HIDDEN pyllbc_ScriptIntegrator
{
public:
    /**
     * Parameter constructor.
     * @param[in] script - the script content.
     */
    pyllbc_ScriptIntegrator(const LLBC_String &script);
    /**
     * Destructor.
     */
    ~pyllbc_ScriptIntegrator();

public:
    /**
     * Integrate script to llbc module.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Integrate();
    
private:
    LLBC_String _script;
    bool _builded;
};

#endif // !__PYLLBC_COM_SCRIPT_INTEGRATOR_H__
