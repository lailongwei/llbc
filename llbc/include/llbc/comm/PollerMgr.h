/**
 * @file    PollerMgr.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/14
 * @version 1.
 *
 * @brief
 */
#ifndef __LLBC_COMM_POLLER_MGR_H__
#define __LLBC_COMM_POLLER_MGR_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Packet;
class LLBC_Socket;
class LLBC_IService;
class LLBC_BasePoller;

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
    void SetService(LLBC_IService *svc);

public:
    /**
     * Startup poller manager.
     * @param[in] count - the poller count.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Start(int count);

    /**
     * Stop poller manager.
     */
    void Stop();

public:
    /**
     * Listen in specified local address(call by service).
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number. 
     * @return int - the new session Id, if return 0, means connect failed.
     *               BE CAREFUL: the return value is a SESSION ID, not error indicator value!!!!!!!!
     */
    int Listen(const char *ip, uint16 port);

    /**
     * Connect to peer address(call by service).
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number. 
     * @return int - the new session Id, if return 0, means connect failed.
     *               BE CAREFUL: the return value is a SESSION ID, not error indicator value!!!!!!!!
     */
    int Connect(const char *ip, uint16 port);

    /**
     * Asynchronous connect to peer address(call by service).
     * @param[in] ip   -              the ip address.
     * @param[in] port -              the port number. 
     * @param[out] pendingSessionId - pending sessionId, when return 0, this output parameter will assign the session Id, otherwise set to 0.
     * @return int - return 0 if success, otherwise return -1.
     *               Note: return 0 is not means the connection was established, 
     *                     it only means post async-conn request to poller success.
     */
    int AsyncConn(const char *ip, uint16 port, int &pendingSessionId);

    /**
     * Send packet.
     * @param[in] packet - the packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_Packet *packet);

    /**
     * Close session.
     * @param[in] sessionId - the session Id.
	 * @param[in] reason    - the session close reason, default is NULL.
     */
    void Close(int sessionId, const char *reason = NULL);

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
    LLBC_IService *_svc;

    int _pollerCount;
    LLBC_BasePoller **_pollers;
    LLBC_SpinLock _pollerLock;

    int _maxSessionId;

    typedef std::map<int, LLBC_Socket *> _PendingAddSocks;
    _PendingAddSocks _pendingAddSocks;

    typedef std::map<int, LLBC_SockAddr_IN> _PendingAsyncConns;
    _PendingAsyncConns _pendingAsyncConns;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_POLLER_MGR_H__

