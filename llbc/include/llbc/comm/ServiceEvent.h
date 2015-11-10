/**
 * @file    ServiceEvent.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_SERVICE_EVENT_H__
#define __LLBC_COMM_SERVICE_EVENT_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/IFacade.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Packet;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The service event type enumeration.
 */
class LLBC_HIDDEN LLBC_SvcEvType
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

        End
    };
};

/**
 * \brief The service event base structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_ServiceEvent
{
    int type;

    LLBC_ServiceEvent(int type);
    virtual ~LLBC_ServiceEvent();
};

/**
 * \brief The session create event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_SessionCreate : public LLBC_ServiceEvent
{
    bool isListen;
    int sessionId;
    LLBC_SockAddr_IN local;
    LLBC_SockAddr_IN peer;
    LLBC_SocketHandle handle;

    LLBC_SvcEv_SessionCreate();
    virtual ~LLBC_SvcEv_SessionCreate();
};

/**
 * \brief The session destroy event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_SessionDestroy : public LLBC_ServiceEvent
{
    int sessionId;
    bool isInitiative;
    LLBC_String reason;

    LLBC_SvcEv_SessionDestroy();
    virtual ~LLBC_SvcEv_SessionDestroy();
};

/**
 * \brief The async-connect result event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_AsyncConn : public LLBC_ServiceEvent
{
    bool connected;
    LLBC_String reason;
    LLBC_SockAddr_IN peer;

    LLBC_SvcEv_AsyncConn();
    virtual ~LLBC_SvcEv_AsyncConn();
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

    int layer;
    int level;
    LLBC_String report;

    LLBC_SvcEv_ProtoReport();
    virtual ~LLBC_SvcEv_ProtoReport();
};

/**
 * \brief The subscribe-event event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_SubscribeEv : public LLBC_ServiceEvent
{
    int id;
    LLBC_String stub;
    LLBC_IDelegate1<LLBC_Event *> *deleg;

    LLBC_SvcEv_SubscribeEv();
    virtual ~LLBC_SvcEv_SubscribeEv();
};

/**
 * \brief The unsubscribe-event event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_UnsubscribeEv : public LLBC_ServiceEvent
{
    int id;
    LLBC_String stub;

    LLBC_SvcEv_UnsubscribeEv();
    virtual ~LLBC_SvcEv_UnsubscribeEv();
};

/**
 * \brief The fire-event event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_SvcEv_FireEv : public LLBC_ServiceEvent
{
    LLBC_Event *ev;

    LLBC_SvcEv_FireEv();
    virtual ~LLBC_SvcEv_FireEv();
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
                                                   LLBC_SocketHandle handle);

    /**
     * Build session destroy event.
     */
    static LLBC_MessageBlock *BuildSessionDestroyEv(int sessionId);

    /**
     * Build async-connect result event.
     */
    static LLBC_MessageBlock *BuildAsyncConnResultEv(bool conneted, 
                                                     const LLBC_String &reason, 
                                                     const LLBC_SockAddr_IN &peer);

    /**
     * Build Data-Arrival event.
     */
    static LLBC_MessageBlock *BuildDataArrivalEv(LLBC_Packet *packet);

    /**
     * Build subscribe-event event.
     */
    static LLBC_MessageBlock *BuildSubscribeEvEv(int id,
                                                 const LLBC_String &stub,
                                                 LLBC_IDelegate1<LLBC_Event *> *deleg);

    /**
     * Build proto-report event.
     */
    static LLBC_MessageBlock *BuildProtoReportEv(int sessionId,
                                                 int layer,
                                                 int level,
                                                 const LLBC_String &report);

    /**
     * Build unsubscribe-event event.
     */
    static LLBC_MessageBlock *BuildUnsubscribeEvEv(int id, const LLBC_String &stub);

    /**
     * Build fire-event event.
     */
    static LLBC_MessageBlock *BuildFireEvEv(LLBC_Event *ev);
};

__LLBC_NS_END

#endif // __LLBC_COMM_SERVICE_EVENT_H__

