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

#pragma once

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

/**
 * \brief The csharp wrap service comp encapsulation.
 */
class CSLLBC_HIDDEN csllbc_Component : public LLBC_Component
{
    typedef csllbc_Delegates _D;

public:
    /**
     * Constructor, must pass all csharp comp delegate methods when construct csComponent object.
     */
    csllbc_Component(_D::Deleg_Comp_OnInit initDeleg,
                     _D::Deleg_Comp_OnDestroy destroyDeleg,
                     _D::Deleg_Comp_OnStart startDeleg,
                     _D::Deleg_Comp_OnStop stopDeleg,
                     _D::Deleg_Comp_OnUpdate updateDeleg,
                     _D::Deleg_Comp_OnIdle idleDeleg,
                     _D::Deleg_Comp_OnSessionCreate sessionCreateDeleg,
                     _D::Deleg_Comp_OnSessionDestroy sessionDestroyDeleg,
                     _D::Deleg_Comp_OnAsyncConnResult asyncConnResultDeleg,
                     _D::Deleg_Comp_OnProtoReport protoReportDeleg,
                     _D::Deleg_Comp_OnUnHandledPacket unHandledPacketDeleg);

public:
    /**
     * All comp event handlers.
     */
    virtual bool OnInit(bool &initFinished);
    virtual void OnDestroy(bool &destroyFinished);

    virtual bool OnStart(bool &startFinished);
    virtual void OnStop(bool &stopFinished);

    virtual void OnUpdate();
    virtual void OnIdle(const LLBC_TimeSpan &idleTime);

public:
    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo);
    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo);
    virtual void OnAsyncConnResult(const LLBC_AsyncConnResult &result);

public:
    virtual void OnProtoReport(const LLBC_ProtoReport &report);
    virtual void OnUnHandledPacket(const LLBC_Packet &packet);

private:
    LLBC_DISABLE_ASSIGNMENT(csllbc_Component);

private:
    _D::Deleg_Comp_OnInit _initDeleg;
    _D::Deleg_Comp_OnDestroy _destroyDeleg;
    _D::Deleg_Comp_OnStart _startDeleg;
    _D::Deleg_Comp_OnStop _stopDeleg;

    _D::Deleg_Comp_OnUpdate _updateDeleg;
    _D::Deleg_Comp_OnIdle _idleDeleg;

    _D::Deleg_Comp_OnSessionCreate _sessionCreateDeleg;
    _D::Deleg_Comp_OnSessionDestroy _sessionDestroyDeleg;
    _D::Deleg_Comp_OnAsyncConnResult _asyncConnResultDeleg;

    _D::Deleg_Comp_OnProtoReport _protoReportDeleg;
    _D::Deleg_Comp_OnUnHandledPacket _unHandledPacketDeleg;
};


