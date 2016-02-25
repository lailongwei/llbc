/**
 * @file    csService.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/comm/csService.h"

LLBC_BEGIN_C_DECL

LLBC_IService *csllbc_Service_Create(int svcType)
{
    if (svcType != static_cast<int>(LLBC_IService::Raw) &&
        svcType != static_cast<int>(LLBC_IService::Normal))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

    return LLBC_IService::Create(static_cast<LLBC_IService::Type>(svcType));
}

void csllbc_Service_Delete(LLBC_IService *svc)
{
    LLBC_XDelete(svc);
}

int csllbc_Service_GetId(LLBC_IService *svc)
{
    return svc->GetId();
}

int csllbc_Service_SetId(LLBC_IService *svc, int svcId)
{
    return svc->SetId(svcId);
}

int csllbc_Service_GetType(LLBC_IService *svc)
{
    return static_cast<int>(svc->GetType());
}

int csllbc_Service_SetDriveMode(LLBC_IService *svc, int driveMode)
{
    return svc->SetDriveMode(static_cast<LLBC_IService::DriveMode>(driveMode));
}

int csllbc_Service_Start(LLBC_IService *svc, int pollerCount)
{
    return svc->Start(pollerCount);
}

void csllbc_Service_Stop(LLBC_IService *svc)
{
    svc->Stop();
}

bool csllbc_Service_IsStarted(LLBC_IService *svc)
{
    return svc->IsStarted();
}

int csllbc_Service_GetFPS(LLBC_IService *svc)
{
    return svc->GetFPS();
}

int csllbc_Service_SetFPS(LLBC_IService *svc, int fps)
{
    return svc->SetFPS(fps);
}

int csllbc_Service_GetFrameInteral(LLBC_IService *svc)
{
    return svc->GetFrameInterval();
}

int csllbc_Service_Listen(LLBC_IService *svc, const char *ip, int port)
{
    return svc->Listen(ip, port);
}

int csllbc_Service_Connect(LLBC_IService *svc, const char *ip, int port)
{
    return svc->Connect(ip, port);
}

int csllbc_Service_AsyncConn(LLBC_IService *svc, const char *ip, int port)
{
    return svc->AsyncConn(ip, port);
}

LLBC_END_C_DECL