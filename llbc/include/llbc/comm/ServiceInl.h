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

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <cxxabi.h>
#endif

#include "llbc/comm/Coder.h"
#include "llbc/comm/Packet.h"

__LLBC_NS_BEGIN

constexpr bool LLBC_ServiceRunningPhase::IsFailedPhase(int runningPhase)
{
    return runningPhase == PreStartFailed ||
        runningPhase == InitCompsFailed ||
        runningPhase == StartCompsFailed;
}

constexpr bool LLBC_ServiceRunningPhase::IsStoppingPhase(int runningPhase)
{
    return runningPhase == Stopping;
}

constexpr bool LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(int runningPhase)
{
    return runningPhase == Stopping || IsFailedPhase(runningPhase);
}

constexpr bool LLBC_ServiceDriveMode::IsValid(int driveMode)
{
    return driveMode >= Begin && driveMode < End;
}

template <typename Comp>
typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value, int>::type
LLBC_Service::AddComponent()
{
    auto comp = new Comp;
    const int ret = AddComponent(comp);
    if (ret != LLBC_OK)
        delete comp;

    return ret;
}

template <typename CompFactory>
typename std::enable_if<std::is_base_of<LLBC_ComponentFactory, CompFactory>::value, int>::type
LLBC_Service::AddComponent()
{
    auto compFactory = new CompFactory;
    LLBC_Defer(delete compFactory);

    auto comp = compFactory->Create(this);
    const int ret = AddComponent(comp);
    if (ret != LLBC_OK)
    {
        LLBC_XDelete(comp);
        return LLBC_FAILED;
    }

    return ret;
}

inline int LLBC_Service::AddComponent(LLBC_ComponentFactory *compFactory)
{
    if (UNLIKELY(!compFactory))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    auto comp = compFactory->Create(this);
    const int ret = AddComponent(comp);
    if (ret != LLBC_OK)
        LLBC_XDelete(comp);
    else
        delete compFactory;

    return ret;
}

inline int LLBC_Service::AddComponent(const LLBC_String &compSharedLibPath,
                                      const LLBC_String &compName)
{
    LLBC_Component *_;
    return AddComponent(compSharedLibPath, compName, _);
}

template <typename Comp>
typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value &&
                        !std::is_same<LLBC_Component, Comp>::value,
                        Comp *>::type
LLBC_Service::GetComponent()
{
    const auto &compList = GetComponentList();
    if (compList.size() <= 32)
    {
        Comp *castComp;
        for (auto *comp : compList)
        {
            if ((castComp = dynamic_cast<Comp *>(comp)) != nullptr)
                return castComp;
        }
    }

    size_t compNameLen;
    char compName[LLBC_CFG_COMM_MAX_COMP_NAME_LEN + 1];
    GetCompName(typeid(Comp).name(), compName, compNameLen);
    return static_cast<Comp *>(GetComponent(LLBC_CString(compName, compNameLen)));
}

template <typename Comp>
typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value &&
                        !std::is_same<LLBC_Component, Comp>::value,
                        const Comp *>::type
LLBC_Service::GetComponent() const
{
    return const_cast<LLBC_Service *>(this)->GetComponent<Comp>();
}

inline const LLBC_Component *LLBC_Service::GetComponent(const LLBC_CString &compName) const
{
    return const_cast<LLBC_Service *>(this)->GetComponent(compName);
}

template <typename CoderFactory>
typename std::enable_if<std::is_base_of<LLBC_CoderFactory, CoderFactory>::value, int>::type
LLBC_Service::AddCoderFactory(int opcode)
{
    auto coderFactory = new CoderFactory;
    const int ret = AddCoderFactory(opcode, coderFactory);
    if (ret != LLBC_OK)
        delete coderFactory;

    return ret;
}

LLBC_FORCE_INLINE int LLBC_Service::Send(int sessionId,
                                         int opcode,
                                         LLBC_Coder *coder,
                                         int status,
                                         uint32 flags)
{
    LLBC_Packet *packet = GetPacketObjectPool().GetObject();
    packet->SetHeader(sessionId, opcode, status, flags);
    packet->SetEncoder(coder);

    return Send(packet);
}

LLBC_FORCE_INLINE int LLBC_Service::Send(int sessionId,
                                         int opcode,
                                         const void *bytes,
                                         size_t len,
                                         int status,
                                         uint32 flags)
{
    // Create packet(from object pool) and send.
    LLBC_Packet *packet = GetPacketObjectPool().GetObject();
    packet->SetHeader(sessionId, opcode, status, flags);
    if (UNLIKELY(packet->Write(bytes, len) != LLBC_OK))
    {
        LLBC_Recycle(packet);
        return LLBC_FAILED;
    }

    return Send(packet);
}

LLBC_FORCE_INLINE int LLBC_Service::Multicast(const LLBC_SessionIds &sessionIds,
                                              int opcode,
                                              LLBC_Coder *coder,
                                              int status,
                                              uint32 flags)
{
    // Argument check.
    if (UNLIKELY(!coder))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Pre-Set last error to LLBC_ERROR_SUCCESS.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Encode coder.
    // TODO: Temporary code, will be optimized in later.
    static thread_local LLBC_Packet pkt;
    if (UNLIKELY(!pkt.GetPayload()))
        pkt.SetPayload(new LLBC_MessageBlock);

    LLBC_MessageBlock *pktPayload = pkt.GetMutablePayload();

    pktPayload->SetWritePos(0);
    if (UNLIKELY(!coder->Encode(pkt)))
    {
        LLBC_Recycle(coder);
        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
            LLBC_SetLastError(LLBC_ERROR_ENCODE);

        return LLBC_FAILED;
    }

    // Recycle coder.
    LLBC_Recycle(coder);

    // Multicast bytes.
    return Multicast(sessionIds,
                     opcode,
                     pktPayload->GetData(),
                     pktPayload->GetWritePos(),
                     status,
                     flags);
}

LLBC_FORCE_INLINE int LLBC_Service::Broadcast(int opcode, LLBC_Coder *coder, int status, uint32 flags)
{
    // Argument check.
    if (UNLIKELY(!coder))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Pre-Set last error to LLBC_ERROR_SUCCESS.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Encode coder.
    // TODO: Temporary code, will be optimized in later.
    static thread_local LLBC_Packet pkt;
    if (UNLIKELY(!pkt.GetPayload()))
        pkt.SetPayload(new LLBC_MessageBlock);

    LLBC_MessageBlock *pktPayload = pkt.GetMutablePayload();

    pktPayload->SetWritePos(0);
    if (UNLIKELY(!coder->Encode(pkt)))
    {
        LLBC_Recycle(coder);
        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
            LLBC_SetLastError(LLBC_ERROR_ENCODE);

        return LLBC_FAILED;
    }

    // Recycle coder.
    LLBC_Recycle(coder);

    // Broadcast bytes.
    return Broadcast(opcode,
                     pktPayload->GetData(),
                     pktPayload->GetWritePos(),
                     status,
					 flags);
}

template <typename ObjType>
int LLBC_Service::Subscribe(int opcode, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    return Subscribe(opcode, LLBC_Delegate<void(LLBC_Packet &)>(obj, method));
}

inline int LLBC_Service::PreSubscribe(int opcode, bool (*func)(LLBC_Packet &))
{
    return PreSubscribe(opcode, LLBC_Delegate<bool(LLBC_Packet &)>(func));
}

template <typename ObjType>
int LLBC_Service::PreSubscribe(int opcode, ObjType *obj, bool (ObjType::*method)(LLBC_Packet &))
{
    return PreSubscribe(opcode, LLBC_Delegate<bool(LLBC_Packet &)>(obj, method));
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
inline int LLBC_Service::UnifyPreSubscribe(bool(*func)(LLBC_Packet &))
{
    return UnifyPreSubscribe(LLBC_Delegate<bool(LLBC_Packet &)>(func));
}

template <typename ObjType>
int LLBC_Service::UnifyPreSubscribe(ObjType *obj, bool (ObjType::*method)(LLBC_Packet &))
{
    return UnifyPreSubscribe(LLBC_Delegate<bool(LLBC_Packet &)>(obj, method));
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
inline int LLBC_Service::SubscribeStatus(int opcode, int status, void(*func)(LLBC_Packet &))
{
    return SubscribeStatus(opcode, status, LLBC_Delegate<void(LLBC_Packet &)>(func));
}

template <typename ObjType>
int LLBC_Service::SubscribeStatus(int opcode, int status, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    if (SubscribeStatus(opcode, status, LLBC_Delegate<void(LLBC_Packet &)>(obj, method)) != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

template <typename ObjType>
LLBC_ListenerStub LLBC_Service::SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event &))
{
    return SubscribeEvent(event, LLBC_Delegate<void(LLBC_Event &)>(obj, method));
}

template <typename ObjType>
int LLBC_Service::Post(ObjType *obj, void (ObjType::*method)(LLBC_Service *))
{
    return Post(LLBC_Delegate<void(LLBC_Service *)>(obj, method));
}

__LLBC_NS_END
