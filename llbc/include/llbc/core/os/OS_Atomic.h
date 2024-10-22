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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Atomic get value operation(32/64 bit version).
 * @param[in] ptr - value pointer.
 * @return sint32/sint64 - the ptr variable pointer to value.
 */
inline sint32 LLBC_AtomicGet(volatile sint32 *ptr)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_add(ptr, 0);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchangeAdd(reinterpret_cast<volatile LONG *>(ptr), 0);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_add(ptr, 0);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_add(ptr, 0);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_add(ptr, 0);
#endif
}

inline sint64 LLBC_AtomicGet(volatile sint64 *ptr)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_add(ptr, 0);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchangeAdd64(ptr, 0);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_add(ptr, 0);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_add(ptr, 0);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_add(ptr, 0);
#endif
}

/**
 * Atomic set value operation(32/64 bit version).
 * @param[in/out] ptr - will set's value pointer.
 * @param[in] value   - value.
 * @return sint32/sint64 - ptr pointer to's value.
 */
inline sint32 LLBC_AtomicSet(volatile sint32 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_lock_test_and_set(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchange(reinterpret_cast<volatile LONG *>(ptr), value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_lock_test_and_set(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_lock_test_and_set(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_lock_test_and_set(ptr, value);
#endif
}

inline sint64 LLBC_AtomicSet(volatile sint64 *ptr, sint64 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_lock_test_and_set(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchange64(ptr, value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_lock_test_and_set(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_lock_test_and_set(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_lock_test_and_set(ptr, value);
#endif
}

/**
 * Perform an atomic addition of increment value to an addend variable.
 * Prevents more than one thread from using the same variable simultaneously.
 * @param[in/out] ptr - pointer to the number that will have the increment number added to it.
 * @param[in] value   - specifies the number to the added to the variable pointed to by addend parameter.
 * @return sint32/sint64 - initial value of the variable pointed to by the addend parameter.
 */
inline sint32 LLBC_AtomicFetchAndAdd(volatile sint32 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_add(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchangeAdd(reinterpret_cast<volatile LONG *>(ptr), value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_add(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_add(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_add(ptr, value);
#endif
}

inline sint64 LLBC_AtomicFetchAndAdd(volatile sint64 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_add(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchangeAdd64(ptr, value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_add(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_add(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_add(ptr, value);
#endif
}

/**
 * Perform an atomic subtraction operation.
 * @param[in/out] ptr - pointer to the number that will have the decrement number sub to it.
 * @param[in] value   - specifies the number to the sub to the variable pointed to by subtraction parameter.
 * @return sint32/sint64 - initial value of the variable poine to by the sub parameter.
 */
inline sint32 LLBC_AtomicFetchAndSub(volatile sint32 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_sub(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchangeAdd(reinterpret_cast<volatile LONG *>(ptr), - value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_sub(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_sub(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_sub(ptr, value);
#endif
}

inline sint64 LLBC_AtomicFetchAndSub(volatile sint64 *ptr, sint64 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_sub(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedExchangeAdd64(ptr, - value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_sub(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_sub(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_sub(ptr, value);
#endif
}

/**
 * Atomically computes a bitwise AND operation.
 * @param[in/out] ptr - pointer to variable to ANDed with value. the result 
 *                      of the operation is stored in the variable.
 * @param[in] value   - specifies the value to be ANDed with the variable that
 *                      is pointed to by ptr.
 * @return sint32/sint64 - returns the original value stored in the variable 
 *                         pointed to by ptr.
 */
inline sint32 LLBC_AtomicFetchAndAnd(volatile sint32 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_and(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return _InterlockedAnd(reinterpret_cast<volatile LONG *>(ptr), value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_and(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_and(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_and(ptr, value);
#endif
}

inline sint64 LLBC_AtomicFetchAndAnd(volatile sint64 *ptr, sint64 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_and(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedAnd64(ptr, value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_and(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_and(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_and(ptr, value);
#endif
}

/**
 * Atomically computes a bitwise OR operation.
 * @param[in/out] ptr - pointer to the variable to be ORed with value, the result
 *                      of the operation is stored in the variable.
 * @param[in] value   - specifies the value to be ORed with the variable that is
 *                      pointed to by ptr.
 * @return sint32/sint64 - returns the originalvalue stored in the variable pointed
 *                         to by ptr.
 */
inline sint32 LLBC_AtomicFetchAndOr(volatile sint32 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_or(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return _InterlockedOr(reinterpret_cast<volatile LONG *>(ptr), value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_or(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_or(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_or(ptr, value);
#endif
}

inline sint64 LLBC_AtomicFetchAndOr(volatile sint64 *ptr, sint64 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_or(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedOr64(ptr, value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_or(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_or(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_or(ptr, value);
#endif
}

/**
 * Atomically computes a bitwise XOR operation.
 * @param[in/out] ptr - pointer to the variable to be XORed with value, the result
 *                      of the operation is stored in the variable.
 * @param[in] value   - specifies the value to be XORed with the variable that is
 *                      pointed to by ptr.
 * @return sint32/sint64 - returns the originalvalue stored in the variable pointed
 *                         to by ptr.
 */
inline sint32 LLBC_AtomicFetchAndXor(volatile sint32 *ptr, sint32 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_xor(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return _InterlockedXor(reinterpret_cast<volatile LONG *>(ptr), value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_xor(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_xor(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_xor(ptr, value);
#endif
}

inline sint64 LLBC_AtomicFetchAndXor(volatile sint64 *ptr, sint64 value)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_fetch_and_xor(ptr, value);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedXor64(ptr, value);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_fetch_and_xor(ptr, value);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_fetch_and_xor(ptr, value);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_fetch_and_xor(ptr, value);
#endif
}

/**
 * Performs an atomic comprarison of specified values and exchanges the values, 
 * base on the outcome of the comparison, the function prevents more than one 
 * thread from using the same variable simultaneously.
 * @param[in/out] ptr   - specifies the address of the destination value.
 * @param[in] exchange  - specifies the exchange value.
 * @param[in] comparand - specifies the value compare to destination.
 * @return sint32/sint64 -  returns the initial value of the ptr.
 */
inline sint32 LLBC_AtomicCompareAndExchange(volatile sint32 *ptr, sint32 exchange, sint32 comparand)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedCompareExchange(reinterpret_cast<volatile LONG *>(ptr), exchange, comparand);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#endif
}

inline sint64 LLBC_AtomicCompareAndExchange(volatile sint64 *ptr, sint64 exchange, sint64 comparand)
{
#if LLBC_TARGET_PLATFORM_LINUX
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::InterlockedCompareExchange64(ptr, exchange, comparand);
#elif LLBC_TARGET_PLATFORM_IPHONE
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#elif LLBC_TARGET_PLATFORM_MAC
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#elif LLBC_TARGET_PLATFORM_ANDROID
    return __sync_val_compare_and_swap(ptr, comparand, exchange);
#endif
}

__LLBC_NS_END
