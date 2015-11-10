/**
 * @file    ServiceImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/10/01
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_SERVICE_H__

__LLBC_NS_BEGIN

inline int LLBC_Service::Send(int sessionId, int opcode, LLBC_ICoder *coder, int status)
{
    return this->Send2(sessionId, opcode, coder, status, NULL);
}

inline int LLBC_Service::Send(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status)
{
    return this->Send2(svcId, sessionId, opcode, coder, status, NULL);
}

inline int LLBC_Service::Send(int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    return this->Send2(sessionId, opcode, bytes, len, status, NULL);
}

inline int LLBC_Service::Send(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    return this->Send2(svcId, sessionId, opcode, bytes, len, status, NULL);
}

inline int LLBC_Service::Multicast(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status)
{
    return this->Multicast2(sessionIds, opcode, coder, status, NULL);
}

inline int LLBC_Service::Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status)
{
    return this->Multicast2(svcId, sessionIds, opcode, coder, status, NULL);
}

inline int LLBC_Service::Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status)
{
    return this->Multicast2(sessionIds, opcode, bytes, len, status, NULL);
}

inline int LLBC_Service::Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status)
{
    return this->Multicast2(svcId, sessionIds, opcode, bytes, len, status, NULL);
}

inline int LLBC_Service::Broadcast(int opcode, LLBC_ICoder *coder, int status)
{
    return this->Broadcast2(opcode, coder, status, NULL);
}

inline int LLBC_Service::Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status)
{
    return this->Broadcast2(svcId, opcode, coder, status, NULL);
}

inline int LLBC_Service::Broadcast(int opcode, const void *bytes, size_t len, int status)
{
    return this->Broadcast2(opcode, bytes, len, status, NULL);
}

inline int LLBC_Service::Broadcast(int svcId, int opcode, const void *bytes, size_t len, int status)
{
    return this->Broadcast2(svcId, opcode, bytes, len, status, NULL);
}

__LLBC_NS_END

#endif // __LLBC_COMM_SERVICE_H__
