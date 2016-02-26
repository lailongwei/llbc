/**
 * @file    ScriptIntegrator.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/12
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/ScriptIntegrator.h"

pyllbc_ScriptIntegrator::pyllbc_ScriptIntegrator(const LLBC_String &script)
: _script(script)
, _builded(false)
{
    ASSERT(!_script.empty() &&
        "python script content must not null!");
}

pyllbc_ScriptIntegrator::~pyllbc_ScriptIntegrator()
{
}

int pyllbc_ScriptIntegrator::Integrate()
{
    if (_builded)
        return LLBC_OK;

    if (PyRun_SimpleString(_script.c_str()) != 0)
    {
        pyllbc_SetError(PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    _builded = true;

    return LLBC_OK;
}
