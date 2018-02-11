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

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc.h" //! Include llbc header to use Startup/Cleanup function.
#include "llbc/application/Application.h"

__LLBC_NS_BEGIN

LLBC_BaseApplication::LLBC_BaseApplication()
: _name()
, _config()

, _services(*LLBC_ServiceMgrSingleton)

, _started(false)
, _waited(false)
{
}

LLBC_BaseApplication::~LLBC_BaseApplication()
{
    Wait();
    Stop();
}

int LLBC_BaseApplication::OnStart(int argc, char *argv[])
{
    return LLBC_OK;
}

void LLBC_BaseApplication::OnWait()
{
}

void LLBC_BaseApplication::OnStop()
{
}

int LLBC_BaseApplication::Start(const char *name, int argc, char *argv[])
{
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_OK;
    }

    // Startup llbc library.
    if (LLBC_Startup() != LLBC_OK)
    {
        if (LLBC_Errno != LLBC_ERROR_REENTRY)
            return LLBC_FAILED;

        LLBC_SetLastError(LLBC_OK);
    }

    // Set application name.
    _name = name;

    // Call OnStart event method.
    if (OnStart(argc, argv) != LLBC_OK)
    {
        LLBC_Cleanup();
        return LLBC_FAILED;
    }

    // Mark started.
    _started = true;

    return LLBC_OK;
}

bool LLBC_BaseApplication::IsStarted() const
{
    return _started;
}

void LLBC_BaseApplication::Wait()
{
    if (!_started || _waited)
        return;

    OnWait();
    _services.Wait();

    _waited = true;
}

void LLBC_BaseApplication::Stop()
{
    if (!_started)
        return;

    if (!_waited)
        Wait();

    OnStop();
    _services.Stop();

    LLBC_Cleanup();

    _started = false;
}

const LLBC_String &LLBC_BaseApplication::GetName() const
{
    return _name;
}

const LLBC_Config &LLBC_BaseApplication::GetConfig() const
{
    return _config;
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
