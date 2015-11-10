/**
 * @file    DictionaryImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/28
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_OBJBASE_DICTIONARY_H__

__LLBC_NS_BEGIN

template <typename _Fn1>
void LLBC_Dictionary::Foreach(_Fn1 &fn)
{
    Iter it = this->Begin();
    for (; it != this->End(); it++)
    {
        fn(it.Elem());
    }
}

template <typename _Fn1>
void LLBC_Dictionary::Foreach(_Fn1 &fn) const
{
    ConstIter it = this->Begin();
    for (; it != this->End(); it++)
    {
        fn(it.Elem());
    }
}

template <typename _Fn2>
void LLBC_Dictionary::Sort(_Fn2 &fn)
{
    if (this->GetSize() < 2)
    {
        return;
    }

    for (LLBC_DictionaryElem *elem = _head->GetElemNext() ; elem != NULL;)
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

        curElem->SetElemPrev(NULL);
        curElem->SetElemNext(NULL);

        // Compare it.
        LLBC_DictionaryElem *prevElem = curElem->GetElemPrev();
        for (; prevElem != NULL; prevElem = prevElem->GetElemPrev())
        {
            if (fn((const LLBC_DictionaryElem *)prevElem, 
                    (const LLBC_DictionaryElem *)curElem))
            {
                break;
            }
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
            {
                prevElem->GetElemNext()->SetElemPrev(curElem);
            }
            else
            {
                _tail = curElem;
            }

            prevElem->SetElemNext(curElem);
        }
    }
}

__LLBC_NS_END

#endif // __LLBC_OBJBASE_DICTIONARY_H__