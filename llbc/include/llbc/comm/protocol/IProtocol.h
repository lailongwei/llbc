/**
 * @file    IProtocol.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_IPROTOCOL_H__
#define __LLBC_COMM_IPROTOCOL_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Session;
class LLBC_ICoderFactory;
class LLBC_ProtocolStack;
class LLBC_IProtocolFilter;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The protocol interface class encapsulation.
 */
class LLBC_EXPORT LLBC_IProtocol
{
    typedef LLBC_IProtocol This;

public:
    typedef std::map<int, LLBC_ICoderFactory *> Coders;

public:
    LLBC_IProtocol();
    virtual ~LLBC_IProtocol();

public:
    /**
     * Get the protocol layer.
     * @return int - the protocol layer.
     */
    virtual int GetLayer() const = 0;

public:
    /**
     * When one connection established, will call this method.
     * @param[in] local - the local address.
     * @param[in] peer  - the peer address.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer) = 0;

    /**
     * When data send, will call this method.
     * @param[in] in             - the in data.
     * @param[out] out           - the out data.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(void *in, void *&out, bool &removeSession) = 0;

    /**
     * When data received, will call this method.
     * @param[in] in             - the in data.
     * @param[out] out           - the out data.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Recv(void *in, void *&out, bool &removeSession) = 0;

private:
    /**
     * Friend class: LLBC_ProtocolStack.
     *  Access methods:
     *      void SetStack()
     *      void SetFilter()
     */
    friend class LLBC_ProtocolStack;

    /**
     * Set protocol stack to protocol.
     * @param[in] stack - the protocol stack.
     */
    void SetStack(LLBC_ProtocolStack *stack);

    /**
     * Set session.
     * @param[in] session - the session.
     */
    void SetSession(LLBC_Session *session);

    /**
     * Set protocol filter to protocol.
     * @param[in] filter - the protocol filter.
     */
    void SetFilter(LLBC_IProtocolFilter *filter);

    /**
     * Set coder factories, only available in Codec-Layer.
     * @param[in] coders - the coder factories pointer.
     * @return int - reutrn 0 if success, otherwise return -1.
     */
    int SetCoders(const Coders *coders);

protected:
    int _sessionId;
    LLBC_Session *_session;
    LLBC_ProtocolStack* _stack;
    LLBC_IProtocolFilter *_filter;
    const Coders *_coders;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_IPROTOCOL_H__
