/**
 * @file    ThreadManagerImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/10
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_THREAD_THREAD_MANAGER_H__

__LLBC_NS_BEGIN

inline void LLBC_ThreadManager::CPURelax()
{
    LLBC_CPURelax();
}

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_THREAD_MANAGER_H__
