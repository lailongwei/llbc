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

#ifdef __LLBC_COMM_ISERVICE_H__

#include "llbc/comm/ICoder.h"
#include "llbc/comm/Packet.h"

__LLBC_NS_BEGIN

template <typename FacadeFactoryCls>
inline int LLBC_IService::RegisterFacade()
{
    FacadeFactoryCls *facadeFactory = LLBC_New(FacadeFactoryCls);
    int ret = RegisterFacade(facadeFactory);
    if (ret != LLBC_OK)
    {
        LLBC_Delete(facadeFactory);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename CoderFactoryCls>
inline int LLBC_IService::RegisterCoder(int opcode)
{
    CoderFactoryCls *coderFactory = LLBC_New(CoderFactoryCls);
    int ret = RegisterCoder(opcode, coderFactory);
    if (ret != LLBC_OK)
    {
        LLBC_Delete(coderFactory);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename FacadeCls>
inline FacadeCls *LLBC_IService::GetFacade()
{
    const LLBC_String facadeName = LLBC_GetTypeName(FacadeCls);
    return static_cast<FacadeCls *>(GetFacade(facadeName));
}

template <typename FacadeCls>
inline FacadeCls *LLBC_IService::GetFacade(const LLBC_String &facadeName)
{
    return static_cast<FacadeCls *>(GetFacade(facadeName));
}

template <typename FacadeCls>
inline std::vector<LLBC_IFacade *> LLBC_IService::GetFacades()
{
    const LLBC_String facadeName = LLBC_GetTypeName(FacadeCls);
    return GetFacades(facadeName);
}

inline int LLBC_IService::Send(int sessionId)
{
    return Send(0, sessionId, 0, static_cast<LLBC_ICoder *>(NULL), 0);
}

inline int LLBC_IService::Send(int sessionId, int opcode)
{
    return Send(0, sessionId, opcode, static_cast<LLBC_ICoder *>(NULL), 0);
}

inline int LLBC_IService::Send(int sessionId, LLBC_ICoder *coder)
{
    return Send(0, sessionId, 0, coder, 0);
}

inline int LLBC_IService::Send(int sessionId, int opcode, LLBC_ICoder *coder)
{
    return Send(0, sessionId, opcode, coder, 0);
}

inline int LLBC_IService::Send(int sessionId, int opcode, LLBC_ICoder *coder, int status)
{
    return Send(0, sessionId, opcode, coder, status);
}

inline int LLBC_IService::Send(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status)
{
    const int svcType = GetType();
    if (svcType == Raw && coder == NULL)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Packet *packet = GetPacketObjectPool().GetObject();
    packet->SetEncoder(coder);

    if (svcType == Raw)
        packet->SetSessionId(sessionId);
    else
        packet->SetHeader(svcId, sessionId, opcode, status);

    return Send(packet);
}

inline int LLBC_IService::Send(int sessionId, const void *bytes, size_t len)
{
    return Send(0, sessionId, 0, bytes, len, 0);
}

inline int LLBC_IService::Send(int sessionId, int opcode, const void *bytes, size_t len)
{
    return Send(0, sessionId, opcode, bytes, len, 0);
}

inline int LLBC_IService::Send(int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    return Send(0, sessionId, opcode, bytes, len, status);
}

inline int LLBC_IService::Send(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    // Validate check.
    const int svcType = GetType();
    if (svcType == Raw && (bytes == NULL || len == 0))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Create packet(from object pool) and send.
    LLBC_Packet *packet = GetPacketObjectPool().GetObject();
    if (svcType == Raw)
        packet->SetSessionId(sessionId);
    else
        packet->SetHeader(svcId, sessionId, opcode, status);

    if (UNLIKELY(packet->Write(bytes, len) != LLBC_OK))
    {
        LLBC_Recycle(packet);
        return LLBC_FAILED;
    }

    return Send(packet);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds)
{
    return Multicast(0, sessionIds, 0, static_cast<LLBC_ICoder *>(NULL), 0);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, int opcode)
{
    return Multicast(0, sessionIds, opcode, static_cast<LLBC_ICoder *>(NULL), 0);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, LLBC_ICoder *coder)
{
    return Multicast(0, sessionIds, 0, coder, 0);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, int opcode, LLBC_ICoder *coder)
{
    return Multicast(0, sessionIds, opcode, coder, 0);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, int opcode, LLBC_ICoder *coder, int status)
{
    return Multicast(0, sessionIds, opcode, coder, status);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, const void *bytes, size_t len)
{
    return Multicast(0, sessionIds, 0, bytes, len, 0);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, int opcode, const void *bytes, size_t len)
{
    return Multicast(0, sessionIds, opcode, bytes, len, 0);
}

template <typename SessionIds>
inline int LLBC_IService::Multicast(const SessionIds &sessionIds, int opcode, const void *bytes, size_t len, int status)
{
    return Multicast(0, sessionIds, opcode, bytes, len, status);
}

inline int LLBC_IService::Broadcast()
{
    return Broadcast(0, 0, static_cast<LLBC_ICoder *>(NULL), 0);
}

inline int LLBC_IService::Broadcast(int opcode)
{
    return Broadcast(0, opcode, static_cast<LLBC_ICoder *>(NULL), 0);
}

inline int LLBC_IService::Broadcast(int opcode, LLBC_ICoder *coder, int status)
{
    return Broadcast(0, opcode, coder, status);
}

inline int LLBC_IService::Broadcast(int opcode, const void *bytes, size_t len)
{
    return Broadcast(0, opcode, bytes, len, 0);
}

inline int LLBC_IService::Broadcast(int opcode, const void *bytes, size_t len, int status)
{
    return Broadcast(0, opcode, bytes, len, status);
}

inline int LLBC_IService::CtrlProtocolStack(int sessionId, int ctrlCmd, const LLBC_Variant &ctrlData)
{
    return CtrlProtocolStack(sessionId, ctrlCmd, ctrlData, reinterpret_cast<LLBC_IDelegate3<void, int, int, const LLBC_Variant &> *>(NULL));
}

inline int LLBC_IService::CtrlProtocolStack(int sessionId, int ctrlCmd, const LLBC_Variant &ctrlData, void (*ctrlDataClearFunc)(int, int, const LLBC_Variant &))
{
    LLBC_IDelegate3<void, int, int, const LLBC_Variant &> *ctrlDataClearDeleg = NULL;
    if (ctrlDataClearFunc)
        ctrlDataClearDeleg = new LLBC_Func3<void, int, int, const LLBC_Variant &>(ctrlDataClearFunc);

    int ret = CtrlProtocolStack(sessionId, ctrlCmd, ctrlData, ctrlDataClearDeleg);
    if (ret != LLBC_OK)
    {
        LLBC_Delete(ctrlDataClearDeleg);
        return LLBC_FAILED;
    };

    return LLBC_OK;
}

template <typename ObjType>
inline int LLBC_IService::CtrlProtocolStack(int sessionId, int ctrlCmd, const LLBC_Variant &ctrlData, ObjType *obj, void ( ObjType::*ctrlDataClearMeth)(int, int, const LLBC_Variant &))
{
    LLBC_IDelegate3<void, int, int, const LLBC_Variant &> *ctrlDataClearDeleg = NULL;
    if (obj && ctrlDataClearMeth)
        ctrlDataClearDeleg = new LLBC_Delegate3<void, ObjType, int, int, const LLBC_Variant &>(obj, ctrlDataClearMeth);

    int ret = CtrlProtocolStack(sessionId, ctrlCmd, ctrlData, ctrlDataClearDeleg);
    if (ret != LLBC_OK)
    {
        LLBC_Delete(ctrlDataClearDeleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

inline int LLBC_IService::Subscribe(int opcode, void(*func)(LLBC_Packet &))
{
    typedef LLBC_Func1<void, LLBC_Packet &> __EvFuncDeleg;

    LLBC_IDelegate1<void, LLBC_Packet &> *deleg = LLBC_New1(__EvFuncDeleg, func);
    if (Subscribe(opcode, deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename ObjType>
inline int LLBC_IService::Subscribe(int opcode, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    typedef LLBC_Delegate1<void, ObjType, LLBC_Packet &> __EvMethDeleg;

    LLBC_IDelegate1<void, LLBC_Packet &> *deleg = LLBC_New2(__EvMethDeleg, obj, method);
    if (Subscribe(opcode, deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

inline int LLBC_IService::PreSubscribe(int opcode, bool (*func)(LLBC_Packet &))
{
    typedef LLBC_Func1<bool, LLBC_Packet &> __PreSubFuncDeleg;

    LLBC_IDelegate1<bool, LLBC_Packet &> *deleg = LLBC_New1(__PreSubFuncDeleg, func);
    if (PreSubscribe(opcode, deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename ObjType>
inline int LLBC_IService::PreSubscribe(int opcode, ObjType *obj, bool (ObjType::*method)(LLBC_Packet &))
{
    typedef LLBC_Delegate1<bool, ObjType, LLBC_Packet &> __PreSubMethDeleg;

    LLBC_IDelegate1<bool, LLBC_Packet &> *deleg = LLBC_New2(__PreSubMethDeleg, obj, method);
    if (PreSubscribe(opcode, deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
inline int LLBC_IService::UnifyPreSubscribe(bool(*func)(LLBC_Packet &))
{
    typedef LLBC_Func1<bool, LLBC_Packet &> __UnifyPreSubFuncDeleg;

    LLBC_IDelegate1<bool, LLBC_Packet &> *deleg = LLBC_New1(__UnifyPreSubFuncDeleg, func);
    if (UnifyPreSubscribe(deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename ObjType>
inline int LLBC_IService::UnifyPreSubscribe(ObjType *obj, bool (ObjType::*method)(LLBC_Packet &))
{
    typedef LLBC_Delegate1<bool, ObjType, LLBC_Packet &> __UnifyPreSubMethDeleg;

    LLBC_IDelegate1<bool, LLBC_Packet &> *deleg = LLBC_New2(__UnifyPreSubMethDeleg, obj, method);
    if (UnifyPreSubscribe(deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
inline int LLBC_IService::SubscribeStatus(int opcode, int status, void(*func)(LLBC_Packet &))
{
    typedef LLBC_Func1<void, LLBC_Packet &> __SubStFuncDeleg;

    LLBC_IDelegate1<void, LLBC_Packet &> *deleg = LLBC_New1(__SubStFuncDeleg, func);
    if (SubscribeStatus(opcode, status, deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename ObjType>
inline int LLBC_IService::SubscribeStatus(int opcode, int status, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    typedef LLBC_Delegate1<void, ObjType, LLBC_Packet &> __SubStMethDeleg;

    LLBC_IDelegate1<void, LLBC_Packet &> *deleg = LLBC_New2(__SubStMethDeleg, obj, method);
    if (SubscribeStatus(opcode, status, deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

inline LLBC_ListenerStub LLBC_IService::SubscribeEvent(int event, void(*func)(LLBC_Event *))
{
    typedef LLBC_Func1<void, LLBC_Event *> __SubEvFuncDeleg;

    LLBC_IDelegate1<void, LLBC_Event *> *deleg = LLBC_New1(__SubEvFuncDeleg, func);
    return SubscribeEvent(event, deleg);
}

template <typename ObjType>
inline LLBC_ListenerStub LLBC_IService::SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event *))
{
    typedef LLBC_Delegate1<void, ObjType, LLBC_Event *> __SubEvMethDeleg;

    LLBC_IDelegate1<void, LLBC_Event *> *deleg = LLBC_New2(__SubEvMethDeleg, obj, method);
    return SubscribeEvent(event, deleg);
}

inline int LLBC_IService::Post(void(*func)(This *, const LLBC_Variant *), LLBC_Variant *data)
{
    typedef LLBC_Func2<void, This *, const LLBC_Variant *> __PostFuncDeleg;

    LLBC_IDelegate2<void, This *, const LLBC_Variant *> *deleg = LLBC_New1(__PostFuncDeleg, func);
    if (Post(deleg) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <typename ObjType>
inline int LLBC_IService::Post(ObjType *obj, void (ObjType::*method)(This *, const LLBC_Variant *), LLBC_Variant *data)
{
    typedef LLBC_Delegate2<void, ObjType, This *, const LLBC_Variant *> __PostMethDeleg;

    LLBC_IDelegate2<void, This *, const LLBC_Variant *> *deleg = LLBC_New2(__PostMethDeleg, obj, method);
    if (Post(deleg, data) != LLBC_OK)
    {
        LLBC_Delete(deleg);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

__LLBC_NS_END

#endif // __LLBC_COMM_ISERVICE_H__

