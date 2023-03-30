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

#include "llbc/core/objbase/Object.h"
#include "llbc/core/objbase/DictionaryElem.h"

/**
 * Previous declare some classes.
 */
__LLBC_NS_BEGIN
class LLBC_ObjectFactory;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The dictionary base const iterator template class encapsulation.
 */
template <bool _Forward>
class LLBC_Dictionary_Const_Iterator_Base
{
public:
    typedef int int_key_type;
    typedef LLBC_String str_key_type;
    typedef LLBC_Object * object_type;
    typedef const object_type cobject_type;
    typedef LLBC_DictionaryElem * pointer;
    typedef LLBC_DictionaryElem * const cpointer;
    typedef LLBC_Dictionary_Const_Iterator_Base<_Forward> iterator;

public:
    LLBC_Dictionary_Const_Iterator_Base()
    : _elem(nullptr)
    {   // construct with null pointer.
    }

    LLBC_Dictionary_Const_Iterator_Base(const iterator &rhs)
    : _elem(rhs._elem)
    {   // construct with another iter.
    }

    explicit LLBC_Dictionary_Const_Iterator_Base(pointer &elem)
    : _elem(elem)
    {   // construct with dictionary element.
    }

    explicit LLBC_Dictionary_Const_Iterator_Base(cpointer &elem)
    : _elem((pointer)elem)
    {   // construct with dictionary element.
    }

    ~LLBC_Dictionary_Const_Iterator_Base()
    {   // do noting.
    }

    bool IsIntKey() const
    {   // return integer key flag.
        return _elem->IsIntKey();
    }

    bool IsStrKey() const
    {   // return string key flag.
        return _elem->IsStrKey();
    }

    const int_key_type &IntKey() const
    {   // return integer type key.
        return _elem->GetIntKey();
    }

    const str_key_type &StrKey() const
    {   // return string type key.
        return _elem->GetStrKey();
    }

    cpointer &Elem() const
    {   // return element.
        return _elem;
    }

    const cobject_type &Obj() const
    {   // return designated object.
        return _elem->GetObject();
    }

    const cobject_type &operator *() const
    {   // return designed object.
        _Checkit();
        return **_elem;
    }

    iterator &operator ++()
    {   // preincrement.
        if (_elem)
        {
            if (_Forward)
                _elem = _elem->GetElemNext();
            else
                _elem = _elem->GetElemPrev();
        }

        return *this;
    }

    iterator operator ++(int)
    {   // postincrement.
        iterator temp(*this);
        ++*this;
        return temp;
    }

    iterator &operator --()
    {   // predecrement.
        if (_elem)
        {
            if (_Forward)
                _elem = _elem->GetElemPrev();
            else
                _elem = _elem->GetElemNext();
        }

        return *this;
    }

    iterator operator --(int)
    {   // postdecrement.
        iterator temp(*this);
        --*this;
        return temp;
    }

    bool operator ==(const iterator &rhs) const
    {   // test for iterator equality.
        return _elem == rhs._elem;
    }

    bool operator !=(const iterator &rhs) const
    {   // test for iterator inequality.
        return !(*this == rhs);
    }

    iterator &operator =(const iterator &rhs)
    {   // copy with another iterator.
        _elem = rhs._elem;
        return *this;
    }

protected:
    void _Checkit() const
    {
#ifdef LLBC_DEBUG
        if (!_elem)
            _Xnullptr();
#endif
    }

protected:
    static void _Xnullptr()
    {
        LLBC_THROW(LLBC_NullPtrException, "invalid LLBC_Dictionary iterator");
    }

protected:
    pointer _elem;
};

/**
 * \brief The dictionary base iterator template class encapsulation.
 */
template <bool _Forward>
class LLBC_Dictionary_Iterator_Base : public LLBC_Dictionary_Const_Iterator_Base<_Forward>
{
    typedef LLBC_Dictionary_Const_Iterator_Base<_Forward> _MyBase;

    using _MyBase::_elem;

public:
    typedef typename _MyBase::int_key_type int_key_type;
    typedef typename _MyBase::str_key_type str_key_type;
    typedef typename _MyBase::object_type object_type;
    typedef typename _MyBase::cobject_type cobject_type;
    typedef typename _MyBase::pointer pointer;
    typedef typename _MyBase::cpointer cpointer;
    typedef LLBC_Dictionary_Iterator_Base<_Forward> iterator;


public:
    LLBC_Dictionary_Iterator_Base()
    {   // construct with null pointer.
    }

    LLBC_Dictionary_Iterator_Base(const iterator &rhs)
    : _MyBase(rhs)
    {   // construct with another iter.
    }

    explicit LLBC_Dictionary_Iterator_Base(pointer elem)
    : _MyBase((typename _MyBase::pointer)elem)
    {   // construct with dictionary element.
    }

    ~LLBC_Dictionary_Iterator_Base()
    {   // do nothing.
    }

    object_type &Obj()
    {   // return designated object.
        return _elem->GetObject();
    }

    pointer &Elem()
    {   // return element.
        return _elem;
    }

    object_type &operator *()
    {   // return designated object.
        return **_elem;
    }

    iterator &operator ++()
    {   // preincrement.
        ++(*(_MyBase *)this);
        return *this;
    }

    const iterator operator ++(int)
    {   // postincrement.
        iterator temp(*this);
        ++*this;
        return temp;
    }

    iterator &operator --()
    {   // predecrement.
        --(*(_MyBase *)this);
        return *this;
    }

    const iterator operator --(int)
    {   // postdecrement.
        iterator temp(*this);
        --*this;
        return temp;
    }

    iterator &operator =(const iterator &rhs)
    {   // copy with another iter.
        (*(_MyBase *)this) = rhs;
        return *this;
    }
};

/**
 * \brief The dictionary class encapsulation.
 */
class LLBC_EXPORT LLBC_Dictionary : public LLBC_Object
{
public:
    typedef long size_type;

    typedef LLBC_Object Obj;
    typedef const LLBC_Object ConstObj;

    typedef LLBC_Dictionary_Iterator_Base<true> Iter;
    typedef LLBC_Dictionary_Const_Iterator_Base<true> ConstIter;
    typedef LLBC_Dictionary_Iterator_Base<false> ReverseIter;
    typedef LLBC_Dictionary_Const_Iterator_Base<false> ConstReverseIter;

public:
    /**
     * Constructor & Destructor.
     */
    explicit LLBC_Dictionary(size_type bucketSize = LLBC_CFG_OBJBASE_DICT_DFT_BUCKET_SIZE);
    virtual ~LLBC_Dictionary();

public:
    /**
     * Cleanup dictionary.
     */
    void Clear();

    /**
     * Get dictionary current size.
     * @return size_type - the dictionary elements count.
     */
    size_type GetSize() const;

    /**
     * Check dictionary empty or not.
     * @return bool - return true if empty, otherwise return false.
     */
    bool IsEmpty() const;

    /**
     * Set dictionary hash bucket size.
     * @param[in] bucketSize - the hash bucket size.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetHashBucketSize(size_type bucketSize);

public:
    /**
     * Insert/Replace/Erase support.
     */
    int Insert(int key, Obj *o);
    int Insert(const LLBC_String &key, Obj *o);

    int Replace(int key, Obj *o);
    int Replace(const LLBC_String &key, Obj *o);
    int Replace(Iter it, Obj *o);

    int Erase(int key);
    int Erase(const LLBC_String &key);
    int Erase(Iter it);
    void Erase(Obj *o);

    /**
     * Find method support.
     */
    Iter Find(int key);
    Iter Find(const LLBC_String &key);

    ConstIter Find(int key) const;
    ConstIter Find(const LLBC_String &key) const;

public:
    /**
     * Iter/ReverseIter support.
     */
    Iter Begin();
    ConstIter Begin() const;

    Iter End();
    ConstIter End() const;

    ReverseIter ReverseBegin();
    ConstReverseIter ReverseBegin() const;

    ReverseIter ReverseEnd();
    ConstReverseIter ReverseEnd() const;

public:
    /**
     * Subscript operations.
     */
    ConstObj *operator[](int key) const;
    ConstObj *operator[](const LLBC_String &key) const;

public:
    /**
     * Foreach support.
     */
    template <typename _Fn1>
    void Foreach(_Fn1 &fn);
    template <typename _Fn1>
    void Foreach(_Fn1 &fn) const;

    /**
     * Sort support.
     */
    template <typename _Fn2>
    void Sort(_Fn2 &fn);

public:
    /**
     * Re-write clone method.
     * @return LLBC_Object * - the clone object.
     */
    LLBC_Object *Clone() const;

    /**
     * Set object create factory.
     * @param[in] factory - the object create factory.
     */
    void SetObjectFactory(LLBC_ObjectFactory *factory);

    /**
     * Serialize/Deserialize support.
     */
    virtual void Serialize(LLBC_Stream &s) const;
    virtual bool DeSerialize(LLBC_Stream &s);

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Dictionary);

private:
    void AddToDoublyLinkedList(LLBC_DictionaryElem *elem);
    void RemoveFromDoublyLinkedList(LLBC_DictionaryElem *elem);

private:
    size_type _size;
    LLBC_DictionaryElem *_head;
    LLBC_DictionaryElem *_tail;

    size_type _bucketSize;
    LLBC_DictionaryElem **_bucket;

    LLBC_ObjectFactory *_objFactory;
};

__LLBC_NS_END

#include "llbc/core/objbase/DictionaryInl.h"


