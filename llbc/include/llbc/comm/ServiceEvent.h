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

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Packet;
class LLBC_SessionCloseInfo;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The service event type enumeration.
 */
class LLBC_HIDDEN LLBC_ServiceEventType
{
public:
    enum
    {
        Begin,
        SessionCreate = Begin,
        SessionDestroy,
        AsyncConnResult,
        DataArrival,
        ProtoReport,

        SubscribeEv,
        UnsubscribeEv,
        FireEv,

        AppPhaseEv,
        AppCfgReloaded,

        End
    };
};

/**
 * \brief The service event base structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_ServiceEvent
{
    int type;

    LLBC_ServiceEvent(int evType);
    virtual ~LLBC_ServiceEvent() = default;
};

/**
 * \brief The session create event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_SessionCreate : public LLBC_ServiceEvent
{
    bool isListen;
    int sessionId;
    int acceptSessionId;
    LLBC_SockAddr_IN local;
    LLBC_SockAddr_IN peer;
    LLBC_SocketHandle handle;

    LLBC_SvcEv_SessionCreate();
};

/**
 * \brief The session destroy event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_SessionDestroy : public LLBC_ServiceEvent
{
    bool isListen;
    int sessionId;
    int acceptSessionId;
    LLBC_SockAddr_IN local;
    LLBC_SockAddr_IN peer;
    LLBC_SocketHandle handle;

    LLBC_SessionCloseInfo *closeInfo;

    LLBC_SvcEv_SessionDestroy();
    virtual ~LLBC_SvcEv_SessionDestroy();
};

/**
 * \brief The async-connect result event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_AsyncConn : public LLBC_ServiceEvent
{
    int sessionId;
    bool connected;
    LLBC_String reason;
    LLBC_SockAddr_IN peer;

    LLBC_SvcEv_AsyncConn();
};

/**
 * \brief The data-arrival event structure enapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_DataArrival : public LLBC_ServiceEvent
{
    LLBC_Packet *packet;

    LLBC_SvcEv_DataArrival();
    virtual ~LLBC_SvcEv_DataArrival();
};

/**
 * \brief The proto-report event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_ProtoReport : public LLBC_ServiceEvent
{
    int sessionId;
    int opcode;

    int layer;
    int level;
    LLBC_String report;

    LLBC_SvcEv_ProtoReport();
};

/**
 * \brief The subscribe-event event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_SubscribeEv : public LLBC_ServiceEvent
{
    int id;
    LLBC_ListenerStub stub;
    LLBC_Delegate<void(LLBC_Event &)> deleg;
    LLBC_EventListener *listener;

    LLBC_SvcEv_SubscribeEv();
    virtual ~LLBC_SvcEv_SubscribeEv();
};

/**
 * \brief The unsubscribe-event event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_UnsubscribeEv : public LLBC_ServiceEvent
{
    int id;
    LLBC_ListenerStub stub;

    LLBC_SvcEv_UnsubscribeEv();
};

/**
 * \brief The fire-event event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_FireEv : public LLBC_ServiceEvent
{
    LLBC_Event *ev;
    LLBC_Delegate<void(LLBC_Event *)> dequeueHandler;

    LLBC_SvcEv_FireEv();
    virtual ~LLBC_SvcEv_FireEv();
};

/**
 * \brief The application phase event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_AppPhaseEv : public LLBC_ServiceEvent
{
    bool earlyStart;
    bool startFail;
    bool startFinish;
    bool earlyStop;

    LLBC_SvcEv_AppPhaseEv();
};

/**
 * \brief The application config reloaded event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_AppCfgReloadedEv : public LLBC_ServiceEvent
{
    int cfgType;
    LLBC_Property propCfg;
    LLBC_Variant nonPropCfg;

    LLBC_SvcEv_AppCfgReloadedEv();
};

/**
 * \brief The service event util class encapsulation.
 *        Use for Build/Destroy service events.
 */
class LLBC_HIDDEN LLBC_SvcEvUtil
{
public:
    /**
     * Build session create event.
     */
    static LLBC_MessageBlock *BuildSessionCreateEv(const LLBC_SockAddr_IN &local,
                                                   const LLBC_SockAddr_IN &peer,
                                                   bool isListen,
                                                   int sessionId,
                                                   int acceptSessionId,
                                                   LLBC_SocketHandle handle);

    /**
     * Build session destroy event.
     */
    static LLBC_MessageBlock *BuildSessionDestroyEv(const LLBC_SockAddr_IN &local,
                                                    const LLBC_SockAddr_IN &peer,
                                                    bool isListen,
                                                    int sessionId,
                                                    int acceptSessionId,
                                                    LLBC_SocketHandle handle,
                                                    LLBC_SessionCloseInfo *closeInfo);

    /**
     * Build async-connect result event.
     */
    static LLBC_MessageBlock *BuildAsyncConnResultEv(int sessionId,
                                                     bool conneted,
                                                     const LLBC_String &reason,
                                                     const LLBC_SockAddr_IN &peer);

    /**
     * Build Data-Arrival event.
     */
    static LLBC_MessageBlock *BuildDataArrivalEv(LLBC_Packet *packet);

    /**
     * Build subscribe-event event.
     */
    static LLBC_MessageBlock *BuildSubscribeEventEv(int id,
                                                    const LLBC_ListenerStub &stub,
                                                    const LLBC_Delegate<void(LLBC_Event &)> &deleg,
                                                    LLBC_EventListener *listener);

    /**
     * Build proto-report event.
     */
    static LLBC_MessageBlock *BuildProtoReportEv(int sessionId,
                                                 int opcode,
                                                 int layer,
                                                 int level,
                                                 const LLBC_String &report);

    /**
     * Build unsubscribe-event event.
     */
    static LLBC_MessageBlock *BuildUnsubscribeEventEv(int id, const LLBC_ListenerStub &stub);

    /**
     * Build fire-event event.
     */
    static LLBC_MessageBlock *BuildFireEventEv(LLBC_Event *ev,
                                               const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler);

    /**
     * Build application phase event.
     */
    static LLBC_MessageBlock *BuildAppPhaseEv(bool earlyStart,
                                              bool startFail,
                                              bool startFinish,
                                              bool earlyStop);

    /**
     * Build application config reload event.
     */
    static LLBC_MessageBlock *BuildAppCfgReloadEv(int cfgType,
                                                  const LLBC_Property &propCfg,
                                                  const LLBC_Variant &nonPropCfg);

public:
    /**
     * Destroy service event block.
     */
    static void DestroyEvBlock(LLBC_MessageBlock *block);
};

__LLBC_NS_END

#include "llbc/comm/ServiceEventInl.h"


