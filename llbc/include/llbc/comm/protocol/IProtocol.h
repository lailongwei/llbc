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
class LLBC_Session;
class LLBC_CoderFactory;
class LLBC_ProtocolStack;
class LLBC_IProtocolFilter;
class LLBC_Service;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The protocol interface class encapsulation.
 */
class LLBC_EXPORT LLBC_IProtocol
{
    typedef LLBC_IProtocol This;

public:
    typedef std::map<int, LLBC_CoderFactory *> Coders;

public:
    LLBC_IProtocol();
    virtual ~LLBC_IProtocol();

public:
    /**
     * Get the protocol layer.
     * @return int - the protocol layer.
     */
    virtual int GetLayer() const = 0;

    /**
    * Get coders.
    * @return const Coders * - the coders.
    */
    const Coders *GetCoders() const;

public:
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

public:
    /**
     * Control protocol layer.
     * @param[in] cmd           - the stack control command(user defined).
     * @param[in] ctrlData      - the stack control data(user defined).
     * @param[in] removeSession - when error occurred, this out param determine remove session or not,
     *                            only used when return false.
     * @return bool - return true if need continue control, otherwise stop ctrl other layers.
     */
    virtual bool Ctrl(int cmd, const LLBC_Variant &ctrlData, bool &removeSession);

protected:
    /**
     * Get session Id.
     * @return int - the session Id.
     */
    int GetSessionId() const;

    /**
     * Get accept session Id.
     * @return int - the accept session Id.
     */
    int GetAcceptSessionId() const;

    /**
     * Get protocol stack.
     * @return LLBC_ProtocolStack * - the protocol stack.
     */
    LLBC_ProtocolStack *GetStack();

    /**
     * Get service.
     * @return LLBC_Service * - the service.
     */
    LLBC_Service *GetService();

private:
    /**
     * Friend class: LLBC_ProtocolStack.
     *  Access methods:
     *      void SetStack()
     *      void SetFilter()
     */
    friend class LLBC_ProtocolStack;

    /**
     * Set session.
     * @param[in] session - the session.
     */
    virtual void SetSession(LLBC_Session *session);

    /**
     * Set protocol stack to protocol.
     * @param[in] stack - the protocol stack.
     */
    virtual void SetStack(LLBC_ProtocolStack *stack);

    /**
     * Set protocol filter to protocol.
     * @param[in] filter - the protocol filter.
     */
    virtual void SetFilter(LLBC_IProtocolFilter *filter);

    /**
     * Set coder factories, only available in Codec-Layer.
     * @param[in] coders - the coder factories pointer.
     * @return int - reutrn 0 if success, otherwise return -1.
     */
    virtual int SetCoders(const Coders *coders);

protected:
    int _sessionId;
    int _acceptSessionId;
    LLBC_Session *_session;
    LLBC_ProtocolStack* _stack;
    LLBC_Service *_svc;
    LLBC_IProtocolFilter *_filter;
    const Coders *_coders;
    LLBC_ObjectPoolInst<LLBC_Packet> *_pktPoolInst;
};

__LLBC_NS_END
