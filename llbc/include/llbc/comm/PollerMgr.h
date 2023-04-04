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
class LLBC_Socket;
class LLBC_Service;
class LLBC_BasePoller;
class LLBC_IProtocolFactory;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The poller manager class encapsulation.
 */
class LLBC_HIDDEN LLBC_PollerMgr
{
public:
    LLBC_PollerMgr();
    virtual ~LLBC_PollerMgr();

public:
    /**
     * Set poller type.
     * @param[in] type - the poller type.
     */
    void SetPollerType(int type);

    /**
     * Set service.
     * @param[in] svc - the service.
     */
    void SetService(LLBC_Service *svc);

public:
    /**
     * Initialize poller manager.
     * @param[in] count - the poller count.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Init(int count);

    /**
     * Finalize poller manager.
     */
    void Finalize();

    /**
     * Startup poller manager.
     * @param[in] count - the poller count.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Start();

    /**
     * Stop poller manager.
     */
    void Stop();

public:
    /**
     * Listen in specified local address(call by service).
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number. 
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id, if return 0, means connect failed.
     *               BE CAREFUL: the return value is a SESSION ID, not error indicator value!!!!!!!!
     */
    int Listen(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory, const LLBC_SessionOpts &sessionOpts);

    /**
     * Connect to peer address(call by service).
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number. 
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id, if return 0, means connect failed.
     *               BE CAREFUL: the return value is a SESSION ID, not error indicator value!!!!!!!!
     */
    int Connect(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory, const LLBC_SessionOpts &sessionOpts);

    /**
     * Asynchronous connect to peer address(call by service).
     * @param[in] ip   -              the ip address.
     * @param[in] port -              the port number. 
     * @param[out] pendingSessionId - pending sessionId, when return 0, this output parameter will assign the session Id, otherwise set to 0.
     * @param[in] protoFactory      - the protocol factory, default use service protocol factory.
     * @param[in] sessionOpts       - the session options.
     * @return int - return 0 if success, otherwise return -1.
     *               Note: return 0 is not means the connection was established, 
     *                     it only means post async-conn request to poller success.
     */
    int AsyncConn(const char *ip,
                  uint16 port,
                  int &pendingSessionId,
                  LLBC_IProtocolFactory *protoFactory,
                  const LLBC_SessionOpts &sessionOpts);

    /**
     * Send packet.
     * @param[in] packet - the packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_Packet *packet);

    /**
     * Close session.
     * @param[in] sessionId - the session Id.
	 * @param[in] reason    - the session close reason, default is nullptr.
     */
    void Close(int sessionId, const char *reason = nullptr);

    /**
     * Control session protocol stack.
     * @param[in] sessionId - the sessionId.
     * @param[in] ctrlCmd   - the stack control type(user defined).
     * @param[in] ctrlData  - the stack control data(user defined).
     * @return int - return 0 if success, otherwise return -1.
     */
    void CtrlProtocolStack(int sessionId,
                           int ctrlCmd,
                           const LLBC_Variant &ctrlData);

private:
    /**
     * Allocate new session Id, call by self or Poller.
     * @return int - the new session Id.
     */
    int AllocSessionId();

    /**
     * Push specific message to poller, call by Poller.
     * @param[in] id    - the poller Id.
     * @param[in] block - the message block.
     * @return int - return 0 if success, otherwise return -1.
     *               Note, this method not set last(for performance sake).
     */
    int PushMsgToPoller(int id, LLBC_MessageBlock *block);

    /**
     * When poller stop, will call this method.
     * @param[in] id - the poller Id.
     */
    void OnPollerStop(int id);

    /**
     * Get address info from given ip and port.
     */
    static int GetAddr(const char *ip, uint16 port, LLBC_SockAddr_IN &addr);

private:
    /**
     * Friend classes.
     */
    friend class LLBC_BasePoller;

private:
    int _type;
    LLBC_Service *_svc;

    int _pollerCount;
    LLBC_BasePoller **_pollers;
    LLBC_SpinLock _pollerLock;

    int _maxSessionId;

    typedef std::map<int, std::pair<LLBC_Socket *, LLBC_SessionOpts> > _PendingAddSocks;
    _PendingAddSocks _pendingAddSocks;
    typedef std::map<int, std::pair<LLBC_SockAddr_IN, LLBC_SessionOpts> > _PendingAsyncConns;
    _PendingAsyncConns _pendingAsyncConns;
};

__LLBC_NS_END
