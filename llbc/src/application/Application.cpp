/**
 * @file    Application.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/17
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc.h" //! Include llbc header to use Startup/Cleanup function.
#include "llbc/application/Application.h"

__LLBC_NS_BEGIN

LLBC_BaseApplication::LLBC_BaseApplication()
: _name()
, _config()

, _services(*LLBC_ServiceMgrSingleton)
{
}

LLBC_BaseApplication::~LLBC_BaseApplication()
{
    Wait();
}

const LLBC_String &LLBC_BaseApplication::GetName() const
{
    return _name;
}

const LLBC_Config &LLBC_BaseApplication::GetConfig() const
{
    return _config;
}

int LLBC_BaseApplication::Initialize(const LLBC_String &name, void *arg)
{
    if(LLBC_Startup() != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    _name = name;
    return LLBC_OK;
}

void LLBC_BaseApplication::Start()
{
}

void LLBC_BaseApplication::Wait()
{
    _services.Wait();

    Stop();
}

void LLBC_BaseApplication::Stop()
{
    _services.Stop();
    LLBC_Cleanup();
}

LLBC_IService *LLBC_BaseApplication::GetService(int id) const
{
    return _services.GetService(id);
}

int LLBC_BaseApplication::RemoveService(int id)
{
    return _services.RemoveService(id);
}

int LLBC_BaseApplication::Send(LLBC_Packet *packet)
{
    LLBC_IService *service = _services.GetService(packet->GetSenderServiceId());
    if(!service)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return service->Send(packet);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
