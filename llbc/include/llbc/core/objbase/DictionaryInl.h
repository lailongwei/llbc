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

__LLBC_NS_BEGIN

template <typename _Fn1>
void LLBC_Dictionary::Foreach(_Fn1 &fn)
{
    Iter it = this->Begin();
    for (; it != this->End(); ++it)
        fn(it.Elem());
}

template <typename _Fn1>
void LLBC_Dictionary::Foreach(_Fn1 &fn) const
{
    ConstIter it = this->Begin();
    for (; it != this->End(); ++it)
        fn(it.Elem());
}

template <typename _Fn2>
void LLBC_Dictionary::Sort(_Fn2 &fn)
{
    if (this->GetSize() < 2)
        return;

    for (LLBC_DictionaryElem *elem = _head->GetElemNext() ; elem != nullptr;)
    {
        LLBC_DictionaryElem *curElem = elem;
        elem = elem->GetElemNext();

        // Remove current object from double-linked list.
        if (curElem->GetElemPrev())
        {
            curElem->GetElemPrev()->SetElemNext(curElem->GetElemNext());
        }
        if (curElem->GetElemNext())
        {
            curElem->GetElemNext()->SetElemPrev(curElem->GetElemPrev());
        }

        curElem->SetElemPrev(nullptr);
        curElem->SetElemNext(nullptr);

        // Compare it.
        LLBC_DictionaryElem *prevElem = curElem->GetElemPrev();
        for (; prevElem != nullptr; prevElem = prevElem->GetElemPrev())
        {
            if (fn(static_cast<const LLBC_DictionaryElem *>(prevElem),
                   static_cast<const LLBC_DictionaryElem *>(curElem)))
                break;
        }

        if (!prevElem)
        {
            curElem->SetElemNext(_head);
            _head->SetElemPrev(curElem);

            _head = curElem;
        }
        else
        {
            curElem->SetElemPrev(prevElem);
            curElem->SetElemNext(prevElem->GetElemNext());
            if (prevElem->GetElemNext())
                prevElem->GetElemNext()->SetElemPrev(curElem);
            else
                _tail = curElem;

            prevElem->SetElemNext(curElem);
        }
    }
}

__LLBC_NS_END

