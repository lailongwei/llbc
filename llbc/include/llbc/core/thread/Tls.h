/**
 * @file    Tls.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_TLS_H__
#define __LLBC_CORE_THREAD_TLS_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Thread local storage class encapsulation, it allow each thread
 *        allocate locations in which to store thread-specific data.
 */
template <typename ValueType>
class LLBC_Tls
{
public:
    LLBC_Tls();
    ~LLBC_Tls();

public:
    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    ValueType *operator ->();

    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    const ValueType *operator ->() const;

    /**
     * Get value associated with the tls.
     * @return ValueType & - value reference.
     */
    ValueType &operator *();

    /**
     * Get value associated with the tls.
     * @return const ValueType & - const value reference.
     */
    const ValueType &operator *() const;

    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    ValueType *GetValue();

    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    const ValueType *GetValue() const;

    /**
     * Set value associated with the tls.
     * @param[in] value - value pointer.
     */
    void SetValue(ValueType *value);

    LLBC_DISABLE_ASSIGNMENT(LLBC_Tls);

private:
    LLBC_TlsHandle _handle;
};

__LLBC_NS_END

#include "llbc/core/thread/TlsImpl.h"

#endif // !__LLBC_CORE_THREAD_TLS_H__
