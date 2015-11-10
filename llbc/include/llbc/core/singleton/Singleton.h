/**
 * @file    Singleton.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/06
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_SINGLETON_SINGLETON_H__
#define __LLBC_CORE_SINGLETON_SINGLETON_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/SpinLock.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare TLS class.
 */
template <typename ValueType>
class LLBC_Tls;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Singleton restricts the instantiation of a class to one object.
 */
template <typename T, typename LockType = LLBC_NS LLBC_SpinLock, bool ThreadUnit = false>
class LLBC_Singleton
{
public:
    /**
     * Get the instance of singleton.
     * @return T * - singleton instance.
     */
    static T *Instance();

    /**
     *  Release instance object.
     */
    void Release();
    
    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Singleton);

private:
    static LockType _lock;

    static T *_instance;
    static LLBC_Tls<T> *_tls;
};

__LLBC_NS_END

#include "llbc/core/singleton/SingletonImpl.h"

#endif // !__LLBC_CORE_SINGLETON_SINGLETON_H__
