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

#ifndef __CSLLBC_COMM_CSFACADE_H__
#define __CSLLBC_COMM_CSFACADE_H__

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

/**
 * \brief The csharp wrap service facade encapsulation.
 */
class CSLLBC_HIDDEN csllbc_Facade : public LLBC_IFacade
{
    typedef csllbc_Delegates _D;

public:
    /**
     * Constructor, must pass all csharp facade delegate methods when construct csFacade object.
     */
    csllbc_Facade(_D::Deleg_Facade_OnInit initDeleg,
                  _D::Deleg_Facade_OnDestroy destroyDeleg,
                  _D::Deleg_Facade_OnStart startDeleg,
                  _D::Deleg_Facade_OnStop stopDeleg,
                  _D::Deleg_Facade_OnUpdate updateDeleg,
                  _D::Deleg_Facade_OnIdle idleDeleg,
                  _D::Deleg_Facade_OnSessionCreate sessionCreateDeleg,
                  _D::Deleg_Facade_OnSessionDestroy sessionDestroyDeleg,
                  _D::Deleg_Facade_OnAsyncConnResult asyncConnResultDeleg,
                  _D::Deleg_Facade_OnProtoReport protoReportDeleg,
                  _D::Deleg_Facade_OnUnHandledPacket unHandledPacketDeleg);

public:
    /**
     * All facade event handlers.
     */
    virtual bool OnInitialize();
    virtual void OnDestroy();

    virtual bool OnStart();
    virtual void OnStop();

    virtual void OnUpdate();
    virtual void OnIdle(int idleTime);

public:
    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo);
    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo);
    virtual void OnAsyncConnResult(const LLBC_AsyncConnResult &result);

public:
    virtual void OnProtoReport(const LLBC_ProtoReport &report);
    virtual void OnUnHandledPacket(const LLBC_Packet &packet);

private:
    LLBC_DISABLE_ASSIGNMENT(csllbc_Facade);

private:
    _D::Deleg_Facade_OnInit _initDeleg;
    _D::Deleg_Facade_OnDestroy _destroyDeleg;
    _D::Deleg_Facade_OnStart _startDeleg;
    _D::Deleg_Facade_OnStop _stopDeleg;

    _D::Deleg_Facade_OnUpdate _updateDeleg;
    _D::Deleg_Facade_OnIdle _idleDeleg;

    _D::Deleg_Facade_OnSessionCreate _sessionCreateDeleg;
    _D::Deleg_Facade_OnSessionDestroy _sessionDestroyDeleg;
    _D::Deleg_Facade_OnAsyncConnResult _asyncConnResultDeleg;

    _D::Deleg_Facade_OnProtoReport _protoReportDeleg;
    _D::Deleg_Facade_OnUnHandledPacket _unHandledPacketDeleg;
};

#endif // !__CSLLBC_COMM_CSFACADE_H__
