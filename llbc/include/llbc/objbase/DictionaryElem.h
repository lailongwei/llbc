/**
 * @file    DictionaryElem.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/27
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_OBJBASE_DICTIONARY_ELEM_H__
#define __LLBC_OBJBASE_DICTIONARY_ELEM_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

#include "llbc/objbase/KeyHashAlgorithm.h"

/**
 * Previous declare some classes.
 */
__LLBC_NS_BEGIN
class LLBC_Object;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The dictionary element class encapsulation.
 */
class LLBC_EXPORT LLBC_DictionaryElem
{
    typedef LLBC_DictionaryElem _MyThis;

public:
    /**
     * Constructors.
     */
    LLBC_DictionaryElem(int key, LLBC_Object *o);
    LLBC_DictionaryElem(const LLBC_String &key, LLBC_Object *o);

    /**
     * Destructor.
     */
    ~LLBC_DictionaryElem();

public:
    /**
     * Check element key type is integer type key or not.
     * @return bool - integer key flag.
     */
    bool IsIntKey() const;

    /**
     * Check element key type is string type key or not.
     * @return bool - string key flag.
     */
    bool IsStrKey() const;

    /**
     * Get integer key.
     */
    int GetIntKey() const;

    /**
     * Get string key.
     * @return const LLBC_String * - the string key.
     */
    const LLBC_String *GetStrKey() const;

    /**
     * Get element key hash value.
     * @return uint32 - the hash value.
     */
    uint32 GetHashValue() const;

    /**
     * Get the element value.
     * @return LLBC_Object *& - the value reference.
     */
    LLBC_Object *&GetObject();
    /**
     * Get the element value(const).
     * @return const LLBC_Object * - the value.
     */
    const LLBC_Object *GetObject() const;

public:
    /**
     * Hash this element to bucket.
     * @param[in] bucket     - element hash bucket.
     * @param[in] bucketSize - bucket size.
     */
    void Hash(LLBC_DictionaryElem **bucket, size_t bucketSize);

    /**
     * Cancel hash.
     */
    void CancelHash();

public:
    /**
     * Get current element's hash bucket container.
     * @return LLBC_DictionaryElem ** - the hash bucket container.
     */
    LLBC_DictionaryElem **GetBucket();

    /**
     * Get bucket size.
     * @return size_t - the bucket size.
     */
    size_t GetBucketSize() const;

public:
    /**
     * Get previous element.
     * @return LLBC_DictionaryElem * - the previous element.
     */
    LLBC_DictionaryElem *GetElemPrev();
    /**
     * Get previous element.
     * @return const LLBC_DictionaryElem * - the previous element.
     */
    const LLBC_DictionaryElem *GetElemPrev() const;
    /**
     * Set previous element.
     * @param[in] prev - the previous element.
     */
    void SetElemPrev(LLBC_DictionaryElem *prev);

    /**
     * Get next element.
     * @return LLBC_DictionaryElem * - the next element.
     */
    LLBC_DictionaryElem *GetElemNext();
    /**
     * Get next element.
     * @return const LLBC_DictionaryElem * - the next element.
     */
    const LLBC_DictionaryElem *GetElemNext() const;
    /**
     * Set next element.
     * @param[in] next - the next element.
     */
    void SetElemNext(LLBC_DictionaryElem *next);

    /**
     * Get previous bucket element.
     * @return LLBC_DictionaryElem * - the previous bucket element.
     */
    LLBC_DictionaryElem *GetBucketElemPrev();
    /**
     * Get previous buckt element.
     * @return LLBC_DictionaryElem * - the previous bucket element.
     */
    const LLBC_DictionaryElem *GetBucketElemPrev() const;
    /**
     * Set previous bucket element.
     * @param[in] prev - the previous bucket element.
     */
    void SetBucketElemPrev(LLBC_DictionaryElem *prev);

    /**
     * Get next bucket element.
     * @return LLBC_DictionaryElem * - the next bucket element.
     */
    LLBC_DictionaryElem *GetBucketElemNext();
    /**
     * Get next bucket element.
     * @return LLBC_DictionaryElem * - the next bucket element.
     */
    const LLBC_DictionaryElem *GetBucketElemNext() const;
    /**
     * Set next bucket element.
     * @param[in] next - the next bucket element.
     */
    void SetBucketElemNext(LLBC_DictionaryElem *next);

public:
    /**
     * Operator *.
     */
    LLBC_Object *&operator *();
    const LLBC_Object *operator *() const;

    LLBC_DISABLE_ASSIGNMENT(LLBC_DictionaryElem);

private:
    int _intKey;
    LLBC_String *_strKey;
    uint32 _hash;

    LLBC_Object *_obj;

    LLBC_DictionaryElem **_bucket;
    size_t _bucketSize;

    _MyThis *_prev;
    _MyThis *_next;

    _MyThis *_bucketPrev;
    _MyThis *_bucketNext;

    const LLBC_KeyHashAlgorithm::HashBase &_hashFun;
};

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_DICTIONARY_ELEM_H__