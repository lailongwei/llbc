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

#include "llbc/comm/Coder.h"
#include "llbc/comm/Packet.h"

__LLBC_NS_BEGIN

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

    auto comp = compFactory->Create();
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

    auto comp = compFactory->Create();
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

    #if LLBC_TARGET_PLATFORM_WIN32
    auto compName = typeid(Comp).name();
    #else
    auto compName = LLBC_GetTypeName(Comp);
    #endif
    const auto colonPos = strrchr(compName, ':');
    return static_cast<Comp *>(GetComponent(colonPos ? colonPos + 1 : compName));
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

inline int LLBC_Service::Send(int sessionId)
{
    return Send(0, sessionId, 0, static_cast<LLBC_Coder *>(nullptr), 0);
}

inline int LLBC_Service::Send(int sessionId, int opcode)
{
    return Send(0, sessionId, opcode, static_cast<LLBC_Coder *>(nullptr), 0);
}

inline int LLBC_Service::Send(int sessionId, LLBC_Coder *coder)
{
    return Send(0, sessionId, 0, coder, 0);
}

inline int LLBC_Service::Send(int sessionId, int opcode, LLBC_Coder *coder)
{
    return Send(0, sessionId, opcode, coder, 0);
}

inline int LLBC_Service::Send(int sessionId, int opcode, LLBC_Coder *coder, int status)
{
    return Send(0, sessionId, opcode, coder, status);
}

inline int LLBC_Service::Send(int svcId, int sessionId, int opcode, LLBC_Coder *coder, int status)
{
    LLBC_Packet *packet = GetPacketObjectPool().GetObject();
    packet->SetEncoder(coder);
    packet->SetHeader(svcId, sessionId, opcode, status);

    return Send(packet);
}

inline int LLBC_Service::Send(int sessionId, const void *bytes, size_t len)
{
    return Send(0, sessionId, 0, bytes, len, 0);
}

inline int LLBC_Service::Send(int sessionId, int opcode, const void *bytes, size_t len)
{
    return Send(0, sessionId, opcode, bytes, len, 0);
}

inline int LLBC_Service::Send(int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    return Send(0, sessionId, opcode, bytes, len, status);
}

inline int LLBC_Service::Send(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    // Create packet(from object pool) and send.
    LLBC_Packet *packet = GetPacketObjectPool().GetObject();
    packet->SetHeader(svcId, sessionId, opcode, status);
    if (UNLIKELY(packet->Write(bytes, len) != LLBC_OK))
    {
        LLBC_Recycle(packet);
        return LLBC_FAILED;
    }

    return Send(packet);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds)
{
    return Multicast(0, sessionIds, 0, static_cast<LLBC_Coder *>(nullptr), 0);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, int opcode)
{
    return Multicast(0, sessionIds, opcode, static_cast<LLBC_Coder *>(nullptr), 0);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, LLBC_Coder *coder)
{
    return Multicast(0, sessionIds, 0, coder, 0);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, int opcode, LLBC_Coder *coder)
{
    return Multicast(0, sessionIds, opcode, coder, 0);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, int opcode, LLBC_Coder *coder, int status)
{
    return Multicast(0, sessionIds, opcode, coder, status);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, const void *bytes, size_t len)
{
    return Multicast(0, sessionIds, 0, bytes, len, 0);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, int opcode, const void *bytes, size_t len)
{
    return Multicast(0, sessionIds, opcode, bytes, len, 0);
}

template <typename SessionIds>
inline int LLBC_Service::Multicast(const SessionIds &sessionIds, int opcode, const void *bytes, size_t len, int status)
{
    return Multicast(0, sessionIds, opcode, bytes, len, status);
}

inline int LLBC_Service::Broadcast()
{
    return Broadcast(0, 0, static_cast<LLBC_Coder *>(nullptr), 0);
}

inline int LLBC_Service::Broadcast(int opcode)
{
    return Broadcast(0, opcode, static_cast<LLBC_Coder *>(nullptr), 0);
}

inline int LLBC_Service::Broadcast(int opcode, LLBC_Coder *coder, int status)
{
    return Broadcast(0, opcode, coder, status);
}

inline int LLBC_Service::Broadcast(int opcode, const void *bytes, size_t len)
{
    return Broadcast(0, opcode, bytes, len, 0);
}

inline int LLBC_Service::Broadcast(int opcode, const void *bytes, size_t len, int status)
{
    return Broadcast(0, opcode, bytes, len, status);
}

template <typename ObjType>
inline int LLBC_Service::Subscribe(int opcode, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    return Subscribe(opcode, LLBC_Delegate<void(LLBC_Packet &)>(obj, method));
}

inline int LLBC_Service::PreSubscribe(int opcode, bool (*func)(LLBC_Packet &))
{
    return PreSubscribe(opcode, LLBC_Delegate<bool(LLBC_Packet &)>(func));
}

template <typename ObjType>
inline int LLBC_Service::PreSubscribe(int opcode, ObjType *obj, bool (ObjType::*method)(LLBC_Packet &))
{
    return PreSubscribe(opcode, LLBC_Delegate<bool(LLBC_Packet &)>(obj, method));
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
inline int LLBC_Service::UnifyPreSubscribe(bool(*func)(LLBC_Packet &))
{
    return UnifyPreSubscribe(LLBC_Delegate<bool(LLBC_Packet &)>(func));
}

template <typename ObjType>
inline int LLBC_Service::UnifyPreSubscribe(ObjType *obj, bool (ObjType::*method)(LLBC_Packet &))
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
inline int LLBC_Service::SubscribeStatus(int opcode, int status, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    if (SubscribeStatus(opcode, status, LLBC_Delegate<void(LLBC_Packet &)>(obj, method)) != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

template <typename ObjType>
inline LLBC_ListenerStub LLBC_Service::SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event &))
{
    return SubscribeEvent(event, LLBC_Delegate<void(LLBC_Event &)>(obj, method));
}

template <typename ObjType>
inline int LLBC_Service::Post(ObjType *obj,
                              void (ObjType::*method)(This *, const LLBC_Variant &),
                              const LLBC_Variant &data)
{
    return Post(LLBC_Delegate<void(This *, const LLBC_Variant &)>(obj, method), data);
}

__LLBC_NS_END
