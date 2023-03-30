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

/**
 * Previous declare some classes.
 */
__LLBC_NS_BEGIN
class LLBC_Array;
class LLBC_ObjectFactory;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The base array const iterator encapsulation.
 */
template <bool _Forward>
class LLBC_Array_Const_Iterator_Base
{
public:
    typedef const LLBC_Object ** pointer;
    typedef long difference_type;
    typedef const LLBC_Object * const & reference;
    typedef LLBC_Array_Const_Iterator_Base<_Forward> iterator;

public:
    LLBC_Array_Const_Iterator_Base()
    : _arr(nullptr)
    , _idx(-1)
    {   // construct with null pointer.
    }

    LLBC_Array_Const_Iterator_Base(const iterator &rhs)
    : _arr(rhs._arr)
    , _idx(rhs._idx)
    {   // construct with another iter.
    }

    LLBC_Array_Const_Iterator_Base(pointer arr, difference_type idx)
    : _arr(arr)
    , _idx(idx)
    {   // construct with arr pointer and off.
    }

    ~LLBC_Array_Const_Iterator_Base()
    {   // do nothing.
    }

    reference operator *() const
    {   // return designated object.
        _Checkit();
        if (_Forward)
            return *(_arr + _idx);

        return *(_arr + _idx - 1);
    }

    iterator &operator =(const iterator &rhs)
    {   // copy with another iter.
        _arr = rhs._arr;
        _idx = rhs._idx;

        return *this;
    }

    iterator &operator ++()
    {   // preincrement.
        _Checkit();
        if (_Forward)
            ++_idx;
        else
            --_idx;

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
        _Checkit();
        if (_Forward)
            --_idx;
        else
            ++_idx;

        return *this;
    }

    const iterator operator --(int)
    {   // postdecrement.
        iterator temp(*this);
        --*this;
        return temp;
    }

    iterator &operator +=(difference_type off)
    {   // increment by integer.
        if (_Forward)
            _idx += off;
        else
            _idx -= off;

        _Checkit();
        return *this;
    }

    const iterator operator +(difference_type off) const
    {   // return this + integer.
        iterator temp(*this);
        return temp += off;
    }

    iterator &operator -=(difference_type off)
    {   // decrement by integer.
        if (_Forward)
            _idx -= off;
        else
            _idx += off;

        _Checkit();
        return *this;
    }

    const iterator operator -(difference_type off) const
    {   // return this - integer.
        iterator temp(*this);
        return temp -= off;
    }

    difference_type operator -(const iterator &rhs) const
    {   // return difference of iterators.
        if (_Forward)
            return _idx - rhs._idx;

        return rhs._idx - _idx;
    }

    reference operator[](difference_type off) const
    {   // subscript
        return *(*this + off);
    }

    bool operator <(const iterator &rhs) const
    {   // test if this < rhs.
#ifdef LLBC_DEBUG
        if (UNLIKELY(_arr != rhs._arr))
            _Xinvarg();
#endif
        return _Forward ? _idx < rhs._idx : _idx > rhs._idx;
    }

    bool operator ==(const iterator &rhs) const
    {   // test for iterator equality.
#ifdef LLBC_DEBUG
        if (UNLIKELY(_arr != rhs._arr))
            _Xinvarg();
#endif
        return _idx == rhs._idx;
    }

    bool operator !=(const iterator &rhs) const
    {   // test for iterator inequality.
        return !(*this == rhs);
    }

    bool operator <=(const iterator &rhs) const
    {   // test if this <= rhs.
        return !(rhs < *this);
    }

    bool operator >(const iterator &rhs) const
    {   // test if this > rhs.
        return rhs < *this;
    }

    bool operator >=(const iterator &rhs) const
    {   // test if this >= rhs.
        return !(*this < rhs);
    }

    friend class LLBC_Array;

protected:
    static void _Xlen()
    {   // report a length exception.
        LLBC_THROW(LLBC_LengthException, "LLBC_Array too long");
    }

    static void _Xunf()
    {   // report an under flow exception.
        LLBC_THROW(LLBC_UnderflowException, "LLBC_Array index underflow");
    }

    static void _Xinvarg()
    {   // report an invalidate argument exception.
        LLBC_THROW(LLBC_InvalidArgException, "LLBC_Array iterator argument invalid");
    }

    static void _Xnullptr()
    {
        LLBC_THROW(LLBC_NullPtrException, "invalid LLBC_Array iterator");
    }

protected:
    void _Checkit() const
    {
#ifdef LLBC_DEBUG
        if (UNLIKELY(!_arr))
            _Xnullptr();
        if (UNLIKELY(_idx < 0))
            _Xunf();
        if (UNLIKELY(_idx == LONG_MAX))
            _Xlen();
#endif
    }

private:
    pointer _arr;
    difference_type _idx;
};

/**
 * \brief The base array iterator encapsulation.
 */
template <bool _Forward>
class LLBC_Array_Iterator_Base : public LLBC_Array_Const_Iterator_Base<_Forward>
{
    typedef LLBC_Array_Const_Iterator_Base<_Forward> _MyBase;

public:
    typedef LLBC_Object ** pointer;
    typedef typename _MyBase::difference_type difference_type;
    typedef LLBC_Object *& reference;
    typedef LLBC_Array_Iterator_Base<_Forward> iterator;

public:
    LLBC_Array_Iterator_Base()
    {   // construct with null pointer.
    }

    LLBC_Array_Iterator_Base(const iterator &rhs)
    : _MyBase(rhs)
    {   // construct with another iter.
    }

    LLBC_Array_Iterator_Base(pointer arr, difference_type idx)
    : _MyBase((typename _MyBase::pointer)arr, idx)
    {   // construct with arr pointer and off.
    }

    ~LLBC_Array_Iterator_Base()
    {   // do nothing.
    }

    reference operator *()
    {   // return designated object.
        return (reference)**(_MyBase *)this;
    }

    iterator &operator =(const iterator &rhs)
    {   // copy with another iter.
        (*(_MyBase *)this) = rhs;
        return *this;
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
    {
        iterator temp(*this);
        --*this;
        return temp;
    }

    iterator &operator +=(difference_type off)
    {   // increment by integer.
        *(_MyBase *)this += off;
        return *this;
    }

    const iterator operator +(difference_type off) const
    {   // return this + integer.
        iterator temp(*this);
        return temp += off;
    }

    iterator &operator -=(difference_type off)
    {   // decrement by integer.
        return *this += -off;
    }

    const iterator operator -(difference_type off) const
    {   // return this - integer.
        iterator temp(*this);
        return temp -= off;
    }

    difference_type operator -(const _MyBase &rhs) const
    {   // return difference of iterators.
        return *(_MyBase *)this - rhs;
    }

    reference operator[](difference_type off)
    {   // subscript.
        return *(*this + off);
    }
};

/**
 * \brief The object array encapsulation.
 */
class LLBC_EXPORT LLBC_Array : public LLBC_Object
{
public:
    typedef long size_type;
    typedef long difference_type;

    typedef LLBC_Object Obj;
    typedef const LLBC_Object ConstObj;

    typedef LLBC_Array_Iterator_Base<true> Iter;
    typedef LLBC_Array_Const_Iterator_Base<true> ConstIter;
    typedef LLBC_Array_Iterator_Base<false> ReverseIter;
    typedef LLBC_Array_Const_Iterator_Base<false> ConstReverseIter;

    typedef std::set<difference_type> IndexSet;
    
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_Array();
    /**
     * Destructor.
     */
    virtual ~LLBC_Array();

public:
    /**
     * Cleanup array.
     */
    void Clear();

    /**
     * Get array size.
     * @return size_type - the array size.
     */
    size_type GetSize() const;

    /**
     * Get capacity.
     * @return size_type - the array capacity.
     */
    size_type GetCapacity() const;

    /**
     * Check array empty or not.
     */
    bool IsEmpty() const;

public:
    /**
     * Insert methods.
     */
    Iter Insert(difference_type n0, Obj *o);
    Iter Insert(Iter it, Obj *o);
    Iter Insert(difference_type n0, const LLBC_Array &other);
    Iter Insert(Iter it, const LLBC_Array &other);

    /**
     * Replace methods.
     */
    Iter Replace(difference_type n0, Obj *o);
    Iter Replace(Iter n0, Obj *o);
    Iter Replace(difference_type n0, difference_type n1, const LLBC_Array &other);
    Iter Replace(Iter n0, Iter n1, const LLBC_Array &other);

    /**
     * Erase methods.
     */
    size_type Erase(Obj *o, bool releaseObj);
    Iter Erase(Iter n0);
    Iter Erase(difference_type n0);
    Iter Erase(Iter n0, Iter n1);
    Iter Erase(difference_type n0, difference_type n1);

    /**
     * Push front/back, Pop front/back support.
     */
    void PushFront(Obj *o);
    void PushFront(Iter it);
    void PushBack(Obj *o);
    void PushBack(Iter it);

    void PopFront();
    void PopBack();

public:
    /**
     * Iterator/ReverseIterator support.
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
     * Helper methods, fetch first object/last object.
     */
    Obj *FirstObject();
    ConstObj *FirstObject() const;

    Obj *LastObject();
    ConstObj *LastObject() const;

    Obj *ObjectAtIndex(difference_type off);
    ConstObj *ObjectAtIndex(difference_type off) const;

    LLBC_Array *ObjectsAtIndexs(const IndexSet &indexs);

public:
    /**
     * Subscript operations.
     */
    Obj *&operator [](difference_type off);
    ConstObj *operator [](difference_type off) const;

public:
    /**
     * Foreach support.
     */
    template <typename _Fn1>
    void Foreach(_Fn1 &fn);
    template <typename _Fn1>
    void Foreach(Iter n0, _Fn1 &fn);
    template <typename _Fn1>
    void Foreach(difference_type n0, _Fn1 &fn);
    template <typename _Fn1>
    void Foreach(Iter n0, Iter n1, _Fn1 &fn);
    template <typename _Fn1>
    void Foreach(difference_type n0, difference_type n1, _Fn1 &fn);

    template <typename _Fn1>
    void Foreach(_Fn1 &fn) const;
    template <typename _Fn1>
    void Foreach(ConstIter n0, _Fn1 &fn) const;
    template <typename _Fn1>
    void Foreach(difference_type n0, _Fn1 &fn) const;
    template <typename _Fn1>
    void Foreach(ConstIter n0, ConstIter n1, _Fn1 &fn) const;
    template <typename _Fn1>
    void Foreach(difference_type n0, difference_type n1, _Fn1 &fn) const;

    /**
     * Sort support.
     */
    template <typename _Fn2>
    void Sort(_Fn2 &fn);
    template <typename _Fn2>
    void Sort(Iter n0, _Fn2 &fn);
    template <typename _Fn2>
    void Sort(difference_type n0, _Fn2 &fn);
    template <typename _Fn2>
    void Sort(Iter n0, Iter n1, _Fn2 &fn);
    template <typename _Fn2>
    void Sort(difference_type n0, difference_type n1, _Fn2 &fn);

public:
    /**
     * Re-write clone method.
     * @return LLBC_Object * - the clone object.
     */
    LLBC_Object *Clone() const;

    /**
     * Set the object factory.
     */
    void SetObjectFactory(LLBC_ObjectFactory *factory);

    /**
     * Serialize/DeSerialize support.
     */
    virtual void Serialize(LLBC_Stream &s) const;
    virtual bool DeSerialize(LLBC_Stream &s);

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Array);

private:
    void Recapacity(size_type cap);

public:
    size_type _size;
    size_type _capacity;
    Obj **_objs;

    LLBC_ObjectFactory *_objFactory;
};

__LLBC_NS_END

#include "llbc/core/objbase/ArrayInl.h"


