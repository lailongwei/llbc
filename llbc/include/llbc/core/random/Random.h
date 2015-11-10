/**
 * @file    Random.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/22
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_RANDOM_RANDOM_H__
#define __LLBC_CORE_RANDOM_RANDOM_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/SpinLock.h"

__LLBC_NS_BEGIN

/**
 * \brief The Mersenne Twister algorithm encapsulation.
 */
class LLBC_EXPORT LLBC_Random
{
public:
    /**
     * Integer in [0, 2^32 - 1].
     * @return uint32 - random value.
     */
    static uint32 RandInt32();

    /**
     * Integer in [0, n].
     * @return uint32 - random value.
     */
    static uint32 RandInt32(uint32 n);

    /**
     * Integer in [m, n].
     * @return uint32 - random value.
     */
    static uint32 RandInt32cmcn(uint32 m, uint32 n);

    /**
     * Integer in [m, n).
     * @return uint32 - random value.
     */
    static uint32 RandInt32cmon(uint32 m, uint32 n);

    /**
     * Integer in (m, n].
     * @return uint32 - random value.
     */
    static uint32 RandInt32omcn(uint32 m, uint32 n);

    /**
     * Integer in (m, n).
     * @return uint32 - random value.
     */
    static uint32 RandInt32omon(uint32 m, uint32 n);

public:
    /**
     * Real number in [0, 1].
     * @return double - random value.
     */
    static double RandRealc0c1();

    /**
     * Real number in [0, n].
     * @return double - random value.
     */
    static double RandRealc0cn(double n);

    /**
     * Real number in [0, 1).
     * @return double - random value.
     */
    static double RandRealc0o1();

    /**
     * Real number in [0, n).
     * @return double - random value.
     */
    static double RandRealc0on(double n);

    /**
     * Real number in (0, 1).
     * @return double - random value.
     */
    static double RandRealo0o1();

    /**
     * Real number in (0, n).
     * @return double - random value.
     */
    static double RandRealo0on(double n);

public:
    /**
     * Access to 53-bit random numbers (capacity of IEEE double precision).
     * @return double - double value.
     */
    static double Rand53Real();

public:
    /**
     * Re-seeding with 32 bitt integer the random class.
     * @param[in] seed - seed value.
     */
    static void Seed(unsigned long seed);

    /**
     * Re-seeding with array the random class.
     * @param[in] array - seed array.
     * @param[in] size  - array size.
     */
    static void Seed(const unsigned long *array, int size);

    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_Random);

private:
    static LLBC_SpinLock _lock;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_RANDOM_RANDOM_H__