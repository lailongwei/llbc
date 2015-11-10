/**
 * @file    ServiceMgr.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/IService.h"
#include "llbc/comm/ServiceMgr.h"

namespace
{
    typedef LLBC_NS LLBC_ServiceMgr This;
}

__LLBC_NS_BEGIN

LLBC_ServiceMgr::LLBC_ServiceMgr()
: _lock()
, _services()
{
}

LLBC_ServiceMgr::~LLBC_ServiceMgr()
{
    // Do nothing.
}

LLBC_IService *LLBC_ServiceMgr::GetService(int id)
{
    LLBC_Guard guard(_lock);
    _Services::iterator it = _services.find(id);
    if (it == _services.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return it->second;
}

int LLBC_ServiceMgr::RemoveService(int id)
{
    LLBC_Guard guard(_lock);
    LLBC_IService *svc = this->GetService(id);
    if (!svc)
    {
        return LLBC_RTN_FAILED;
    }
    else if (This::InTls(svc))
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_RTN_FAILED;
    }

    LLBC_Delete(svc);
    return LLBC_RTN_OK;
}

int LLBC_ServiceMgr::Wait()
{
    _lock.Lock();
    if (This::InTls(_services))
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_RTN_FAILED;
    }

    _Services svcs = _services;
    _lock.Unlock();

    for (_Services::iterator it = svcs.begin();
         it != svcs.end();
         it++)
    {
        _lock.Lock();
        if (_services.find(it->first) == _services.end())
        {
            _lock.Unlock();
            continue;
        }
        _lock.Unlock();

        // Ignore return value.
        it->second->Wait();
    }

    return LLBC_RTN_OK;
}

int LLBC_ServiceMgr::Stop()
{
    _lock.Lock();
    if (This::InTls(_services))
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_RTN_FAILED;
    }

    _Services svcs = _services;
    _lock.Unlock();

    for (_Services::iterator it = svcs.begin();
         it != svcs.end();
         it++)
    {
        _lock.Lock();
        if (_services.find(it->first) == _services.end())
        {
            _lock.Unlock();
            continue;
        }
        _lock.Unlock();
        
        it->second->Stop();
    }

    return LLBC_RTN_OK;
}

bool LLBC_ServiceMgr::InTls(const LLBC_IService *svc)
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    void **svcs = tls->commTls.services;
    for (int i = 0;
         i <= LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
         i++)
        if (reinterpret_cast<LLBC_IService *>(svcs[i]) == svc)
            return true;

    return false;
}

bool LLBC_ServiceMgr::InTls(const This::_Services &svcs)
{
    for (_Services::const_iterator it = svcs.begin();
         it != svcs.end();
         it++)
        if (This::InTls(it->second))
            return true;

    return false;
}

void LLBC_ServiceMgr::OnServiceStart(LLBC_IService *svc)
{
    LLBC_Guard guard(_lock);
    _services.insert(std::make_pair(svc->GetId(), svc));
}

void LLBC_ServiceMgr::OnServiceStop(LLBC_IService *svc)
{
    LLBC_Guard guard(_lock);
    _services.erase(svc->GetId());
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
