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

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/PackLemma_Raw.h"
#include "pyllbc/common/PackLemma_Sequence.h"

namespace
{
    typedef pyllbc_PackLemma Base;

    typedef Base::Type Type;
    typedef Base::State State;
    typedef Base::Symbol Symbol;
}

pyllbc_PackLemma_Sequence::pyllbc_PackLemma_Sequence(PyObject *compileEnv)
: Base(compileEnv)
{
}

pyllbc_PackLemma_Sequence::~pyllbc_PackLemma_Sequence()
{
    LLBC_STLHelper::DeleteContainer(_lemmas);
}

Type pyllbc_PackLemma_Sequence::GetType() const
{
    return Base::SequenceType;
}

bool pyllbc_PackLemma_Sequence::IsSerializable() const
{
    return true;
}

int pyllbc_PackLemma_Sequence::Process(Symbol ch, Symbol nextCh)
{   
    if (_state == Base::Done || _state == Base::Error)
    {
        pyllbc_SetError(
            "sequence-lemma is done or error, could not continuing to process format symbol");
        return LLBC_FAILED;
    }
    else if (_state == Base::Begin)
    {
        const SymbolGroup &seqBegin = GroupedSymbol::SeqBegin();
        if (seqBegin.find(ch) == seqBegin.end())
        {
            _state = Base::Error;

            LLBC_String errStr;
            pyllbc_SetError(errStr.format(
                "sequence-lemma expect sequence begin character, got %c", ch));

            return LLBC_FAILED;
        }

        _seqType = ch;
        _state = Base::Accepting;

        _str.append(1, static_cast<char>(_seqType));

        return LLBC_OK;
    }

    const SymbolGroup &raw = GroupedSymbol::Raw();
    const SymbolGroup &listPair = GroupedSymbol::ListPair();
    const SymbolGroup &tuplePair = GroupedSymbol::TuplePair();
    if (ch != _seqType &&
        ((listPair.find(_seqType) != listPair.end() &&
        listPair.find(ch) != listPair.end()) ||
        (tuplePair.find(_seqType) != tuplePair.end() &&
        tuplePair.find(ch) != tuplePair.end())))
    {
        if (UNLIKELY(_lemmas.empty()))
        {
            _state = Base::Error;

            LLBC_String errMsg;
            pyllbc_SetError(errMsg.format(
                "sequence-lemma not exist sub lemma to describe sequence element type"));

            return LLBC_FAILED;
        }

        _state = Base::Done;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }

    if (raw.find(ch) == raw.end())
    {
        _state = Base::Error;

        LLBC_String errMsg;
        pyllbc_SetError(errMsg.format(
            "bad format character in sequence-lemma, format character: %c", ch));

        return LLBC_FAILED;
    }

    Base *lemma = new pyllbc_PackLemma_Raw;
    if (lemma->Process(ch) != LLBC_OK)
    {
        delete lemma;
        _state = Base::Error;

        return LLBC_FAILED;
    }

    _lemmas.push_back(lemma);
    _str.append(1, static_cast<char>(ch));

    return LLBC_OK;
}

int pyllbc_PackLemma_Sequence::Process(Base *lemma)
{
    if (lemma->GetState() != Base::Done)
    {
        _state = Base::Error;
        pyllbc_SetError("will accept lemma state not done, sequence-lemma could not accept it");

        return LLBC_FAILED;
    }
    else if(lemma->GetType() == Base::TopType)
    {
        _state = Base::Error;
        pyllbc_SetError("sequence-lemma cound not accept Top type lemma");

        return LLBC_FAILED;
    }
    else if(!lemma->IsSerializable())
    {
        _state = Base::Error;
        pyllbc_SetError("sequence-lemma could not accept UnSerializable type lemma");

        return LLBC_FAILED;
    }

    if (_state != Base::Accepting)
    {
        _state = Base::Error;
        pyllbc_SetError("sequence-lemma not in accepting state, could not accept lemma");

        return LLBC_FAILED;
    }

    _lemmas.push_back(lemma);
    _str.append (lemma->ToString());

    return LLBC_OK;
}

PyObject *pyllbc_PackLemma_Sequence::Read(pyllbc_Stream *stream)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("sequence-lemma state not done, could not execute unpack");
        return nullptr;
    }

    int len;
    LLBC_Stream &llbcStream = stream->GetLLBCStream();
    if (!llbcStream.Read(len))
    {
        pyllbc_SetError("not enough bytes to unpack sequence(head-part)");
        return nullptr;
    }

    if (UNLIKELY(len < 0))
    {
        pyllbc_SetError("when unpacking sequence data, unpack len < 0");
        return nullptr;
    }

    const int lemmasCount = static_cast<int>(_lemmas.size());
    if (_lemmas.size() > 1 && len != lemmasCount)
    {
        pyllbc_SetError("when unpacking sequence data, format string len > 1, "
            "and sequence head len(from stream) not equal the format stream len");
        return nullptr;
    }

    PyObject *seq;
    if (_seqType == Base::ListBegin)
        seq = PyList_New(len);
    else
        seq = PyTuple_New(len);

    for (int i = 0; i < len; ++i)
    {
        Base *lemma = _lemmas.at(lemmasCount > 1 ? i : 0);
        PyObject *elem = lemma->Read(stream);
        if (!elem)
        {
            Py_DECREF(seq);
            return nullptr;
        }

        if (_seqType == Base::ListBegin)
            PyList_SetItem(seq, i, elem);
        else
            PyTuple_SetItem(seq, i, elem);
    }

    return seq;
}

int pyllbc_PackLemma_Sequence::Write(pyllbc_Stream *stream, PyObject *values)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("sequence-lemma state not done, could not packing data");
        return LLBC_FAILED;
    }
    else if (!PySequence_Check(values))
    {
        pyllbc_SetError("will pack data not iteratble");
        return LLBC_FAILED;
    }

    const Py_ssize_t len = PySequence_Size(values);
    const Py_ssize_t lemmaCount = static_cast<Py_ssize_t>(_lemmas.size());
    if (lemmaCount > 1 && len != lemmaCount)
    {
        pyllbc_SetError("when packing sequence data, format string size > 1, "
            "but sequence len not equal the format string size");
        return LLBC_FAILED;
    }

    LLBC_Stream &llbcStream = stream->GetLLBCStream();

#if LLBC_TARGET_PROCESSOR_X86_64
    llbcStream.Write(static_cast<sint32>(len));
#else
    llbcStream.Write(len);
#endif

    for (Py_ssize_t i = 0; i < len; ++i)
    {
        Base *lemma = _lemmas.at(lemmaCount > 1 ? i : 0);
        PyObject *elem = PySequence_ITEM(values, i);

        if (lemma->Write(stream, elem) != LLBC_OK)
        {
            Py_DECREF(elem);
            return LLBC_FAILED;
        }

        Py_DECREF(elem);
    }

    return LLBC_OK;
}
