/**
 * @file    KeyHashAlgorithm.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/27
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_OBJBASE_KEY_HASH_ALGORITHM_H__
#define __LLBC_OBJBASE_KEY_HASH_ALGORITHM_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The key hash algorithm type enumeration.
 */
class LLBC_EXPORT LLBC_KeyHashAlgorithmType
{
    typedef LLBC_KeyHashAlgorithmType _This;

public:
    enum
    {
        Begin, 

        SDBM = Begin,
        RS,
        JS,
        PJ,
        ELF,
        BKDR,
        DJB,
        AP,

        End
    };

    /**
     * Check given type is legal or not.
     * @param[in] type - algorithm type.
     * @return bool - return true if legal, otherwise return false.
     */
    static bool IsLegal(int type);

    /**
     * Get type describe.
     * @param[in] type - algorithm type.
     * @return const LLBC_String & - the type describe.
     */
    static const LLBC_String &Type2Str(int type);

    /**
     * Get type by algorithm describe.
     * @param[in] algoDesc - the algorithm describe.
     * @return int - the algorithm type.
     */
    static int Str2Type(const LLBC_String &algoDesc);
};

/**
 * \brief The key hash algorithms encapsulation.
 */
class LLBC_EXPORT LLBC_KeyHashAlgorithm
{
public:
    /**
     * All hash algorithm base function object.
     */
    struct HashBase : 
        public LLBC_BinaryFunction<const void *, size_t, uint32>
    {
        virtual ~HashBase() {  }
        typedef LLBC_BinaryFunction<const void *, size_t, uint32>::Argument1_Type Argument1_Type;
        typedef LLBC_BinaryFunction<const void *, size_t, uint32>::Argument2_Type Argument2_Type;
        typedef LLBC_BinaryFunction<const void *, size_t, uint32>::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const = 0;
    };

public:
    LLBC_KeyHashAlgorithm();
    ~LLBC_KeyHashAlgorithm();

public:
    /**
     * Get algorithm by type.
     * @return const HashBase * - the base hash function object reference.
     */
    const HashBase *GetAlgorithm(int type) const;
    /**
     * Get algorithm by type describe.
     * @return const HashBase * - the base hash function object reference.
     */
    const HashBase *GetAlgorithm(const LLBC_String &algoDesc) const;

private:
    /**
     * SDBM hash algorithm.
     */
    struct SDBMHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * RS hash algorithm.
     */
    struct RSHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * JS hash algorithm.
     */
    struct JSHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * P. J. Weinberger hash algorithm.
     */
    struct PJHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * ELF hash algorithm.
     */
    struct ELFHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * BKDR hash algorithm.
     */
    struct BKDRHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * DJB hash algorithm.
     */
    struct DJBHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

    /**
     * AP hash algorithm.
     */
    struct APHash : public HashBase
    {
        typedef HashBase::Argument1_Type Argument1_Type;
        typedef HashBase::Argument2_Type Argument2_Type;
        typedef HashBase::Result_Type Result_Type;

        virtual Result_Type operator()(Argument1_Type buf, Argument2_Type size) const;
    };

private:
    HashBase *m_algos[LLBC_KeyHashAlgorithmType::End];
};

// Singleton macro define.
template class LLBC_EXPORT LLBC_Singleton<LLBC_KeyHashAlgorithm>;
#define LLBC_KeyHashAlgorithmSingleton LLBC_Singleton<LLBC_KeyHashAlgorithm>::Instance()

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_KEY_HASH_ALGORITHM_H__
