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
#include "pyllbc/common/PyTypeDetector.h"
#include "pyllbc/common/PackLemma_Top.h"

namespace
{
    typedef pyllbc_PackLemma Base;

    typedef Base::Type Type;
    typedef Base::State State;
    typedef Base::Symbol Symbol;
}

pyllbc_PackLemma_Top::pyllbc_PackLemma_Top(PyObject *compileEnv)
: Base(compileEnv)
{
}

pyllbc_PackLemma_Top::~pyllbc_PackLemma_Top()
{
    LLBC_STLHelper::DeleteContainer(_lemmas);
}

Type pyllbc_PackLemma_Top::GetType() const
{
    return Base::TopType;
}

State pyllbc_PackLemma_Top::GetState() const
{
    if (_state == Base::Begin || 
        _state == Base::Accepting || 
        _state == Base::Done)
        return Base::Done;

    return _state;
}

bool pyllbc_PackLemma_Top::IsSerializable() const
{
    return true;
}

bool pyllbc_PackLemma_Top::IsDone() const
{
    return GetState() == Base::Done;
}

int pyllbc_PackLemma_Top::Process(Symbol ch, Symbol nextCh)
{
    _state = Base::Error;
    pyllbc_SetError("top level pack-lemma could not accept any character symbols");

    return LLBC_FAILED;
}

int pyllbc_PackLemma_Top::Process(Base *lemma)
{
    if (!lemma->IsDone())
    {
        _state = Base::Error;
        pyllbc_SetError("top level lemma could not accept not done lemma");

        return LLBC_FAILED;
    }

    _state = Base::Accepting;
    _lemmas.push_back(lemma);

    _str.append(lemma->ToString());

    return LLBC_OK;
}

PyObject *pyllbc_PackLemma_Top::Read(pyllbc_Stream *stream)
{
    if (UNLIKELY(!IsDone()))
    {
        pyllbc_SetError("top-lemma not done, could not unpack data");
        return nullptr;
    }

    size_t idx = 0;
    PyObject *values = PyTuple_New(_lemmas.size());
    for (; idx < _lemmas.size(); ++idx)
    {
        PyObject *value;
        Base *lemma = _lemmas[idx];
        if (!(value = lemma->Read(stream)))
            break;

        PyTuple_SetItem(values, idx, value);
    }

    if (idx != _lemmas.size())
    {
        Py_DECREF(values);
        values = nullptr;
    }

    return values;
}

int pyllbc_PackLemma_Top::Write(pyllbc_Stream *stream, PyObject *values)
{
    if (UNLIKELY(!IsDone()))
    {
        pyllbc_SetError("top-lemma not done, could not pack data");
        return LLBC_FAILED;
    }

    const bool valuesIsNone = pyllbc_TypeDetector::IsNone(values);
    if (valuesIsNone)
    {
        if (!_lemmas.empty())
        {
            pyllbc_SetError("not found any values to pack, but has been specified format character symbol");
            return LLBC_FAILED;
        }

        return LLBC_OK;
    }
    else if (!pyllbc_TypeDetector::IsSequence(values))
    {
        pyllbc_SetError("will pack data not iterable");
        return LLBC_FAILED;
    }
    
    const Py_ssize_t seqSize = PySequence_Size(values);
    if (seqSize != static_cast<Py_ssize_t>(_lemmas.size()))
    {
        LLBC_String errStr;
        pyllbc_SetError(errStr.format(
            "will pack data sequence size[%ld] not equal format character size[%d]", 
            seqSize, _lemmas.size()));

        return LLBC_FAILED;
    }

    for (Py_ssize_t i = 0; i < seqSize; ++i)
    {
        Base *lemma = _lemmas.at(i);
        PyObject *obj = PySequence_GetItem(values, i);
        if (lemma->Write(stream, obj) != LLBC_OK)
        {
            Py_DECREF(obj);
            return LLBC_FAILED;
        }

        Py_DECREF(obj);
    }

    return LLBC_OK;
}
