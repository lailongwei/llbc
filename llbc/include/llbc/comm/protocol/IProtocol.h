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
    LLBC_INLINE void SetStack(LLBC_ProtocolStack *stack);

    /**
     * Set protocol filter to protocol.
     * @param[in] filter - the protocol filter.
     */
    LLBC_INLINE void SetFilter(LLBC_IProtocolFilter *filter);

    /**
     * Set coder factories, only available in Codec-Layer.
     * @param[in] coders - the coder factories pointer.
     * @return int - reutrn 0 if success, otherwise return -1.
     */
    LLBC_INLINE int SetCoders(const Coders *coders);

protected:
    LLBC_ProtocolStack* _stack;
    LLBC_IProtocolFilter *_filter;
    const Coders *_coders;
};

/**
 * \brief The protocol factory interface encapsulation.
 */
class LLBC_EXPORT LLBC_IProtocolFactory
{
public:
    virtual ~LLBC_IProtocolFactory() {  }

public:
    /**
     * Create specific layer protocol.
     * @return LLBC_IProtocol * - the protocol pointer.
     */
    virtual LLBC_IProtocol *Create(int layer) const = 0;
};

/**
 * \brief The llbc library normal protocol factory encapsulation.
 */
class LLBC_HIDDEN LLBC_NormalProtocolFactory : public LLBC_IProtocolFactory
{
public:
    /**
     * Create specific layer protocol.
     * @return LLBC_IProtocol * - the protocol pointer.
     */
    virtual LLBC_IProtocol *Create(int layer) const;
};

/**
 * \brief The llbc library raw protocol factory encapsulation.
 */
class LLBC_HIDDEN LLBC_RawProtocolFactory : public LLBC_IProtocolFactory
{
public:
    /**
     * Create specific layer protocol.
     * @return LLBC_IProtocol * - the protocol pointer.
     */
    virtual LLBC_IProtocol *Create(int layer) const;
};

__LLBC_NS_END

#include "llbc/comm/protocol/IProtocolImpl.h"

#endif // !__LLBC_COMM_IPROTOCOL_H__
