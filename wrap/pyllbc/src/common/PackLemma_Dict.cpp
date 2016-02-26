/**
 * @file    PackLemma_Dict.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/02
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/PackLemma_Raw.h"
#include "pyllbc/common/PackLemma_Dict.h"

namespace
{
    typedef pyllbc_PackLemma Base;

    typedef Base::Type Type;
    typedef Base::State State;
    typedef Base::Symbol Symbol;
}

pyllbc_PackLemma_Dict::pyllbc_PackLemma_Dict(PyObject *compileEnv)
: Base(compileEnv)
, _gotKwSep(false)

, _keyLemma(NULL)
, _valueLemma(NULL)
{
}

pyllbc_PackLemma_Dict::~pyllbc_PackLemma_Dict()
{
    LLBC_XDelete(_keyLemma);
    LLBC_XDelete(_valueLemma);
}

Type pyllbc_PackLemma_Dict::GetType() const
{
    return Base::DictType;
}

bool pyllbc_PackLemma_Dict::IsSerializable() const
{
    return true;
}

int pyllbc_PackLemma_Dict::Process(Symbol ch, Symbol nextCh)
{
    // State logic.
    if (_state == Base::Done && _state == Base::Error)
    {
        pyllbc_SetError("dict-lemma state is done or error, could not continuing to parse format string");
        return LLBC_FAILED;
    }

    if (_state == Base::Begin)
    {
        if (ch != Base::DictBegin)
        {
            _state = Base::Error;
            pyllbc_SetError("dict-lemma expect dict begin character'{', got %c", ch);

            return LLBC_FAILED;
        }

        _state = Base::Accepting;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }

    // Key-Word separator & dict close character('}') logic.
    if (ch == Base::DictKWSep)
    {
        if (!_keyLemma)
        {
            _state = Base::Error;
            pyllbc_SetError("dict-lemma expect key lemma, got key-word separator");

            return LLBC_FAILED;
        }
        else if (_valueLemma)
        {
            _state = Base::Error;
            pyllbc_SetError("dict-lemma expect dict close character '}', got key-word separator");

            return LLBC_FAILED;
        }

        _gotKwSep = true;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }
    else if (ch == Base::DictEnd)
    {
        if (!_keyLemma || !_valueLemma)
        {
            _state = Base::Error;
            pyllbc_SetError("dict-lemma not done, but got dict close character '}'");

            return LLBC_FAILED;
        }

        _state = Base::Done;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }

    const SymbolGroup &raw = GroupedSymbol::Raw();
    if (raw.find(ch) == raw.end())
    {
        _state = Base::Error;
        pyllbc_SetError("dict-lemma could direct process non-raw type format character: %c", ch);

        return LLBC_FAILED;
    }

    Base *lemma = LLBC_New(pyllbc_PackLemma_Raw);
    if (lemma->Process(ch) != LLBC_OK)
    {
        delete lemma;
        _state = Base::Error;

        return LLBC_FAILED;
    }

    if (Process(lemma) != LLBC_OK)
    {
        delete lemma;
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_PackLemma_Dict::Process(Base *lemma)
{
    // Param check.
    if (!lemma->IsSerializable())
    {
        _state = Base::Error;
        pyllbc_SetError("dict-lemma could not accept UnSerializable lemma");

        return LLBC_FAILED;
    }

    // State check.
    if (_state == Base::Done || _state == Base::Error)
    {
        pyllbc_SetError("dict-lemma state is done or error, could not continuing to parse format string");
        return LLBC_FAILED;
    }
    else if (_state == Base::Begin)
    {
        _state = Base::Error;
        pyllbc_SetError("dict-lemma expect dict begin character: '{', got lemma");

        return LLBC_FAILED;
    }

    // Key, Value logic check.
    if (_valueLemma)
    {
        _state = Base::Error;
        pyllbc_SetError("dict-lemma expect dict close character'}', but got lemma");

        return LLBC_FAILED;
    }
    else if (_keyLemma && !_gotKwSep)
    {
        _state = Base::Error;
        pyllbc_SetError("dict-lemma expect dict key-word separator':', but got lemma");
        
        return LLBC_FAILED;
    }

    (!_keyLemma ? _keyLemma : _valueLemma) = lemma;
    _str.append(lemma->ToString());

    return LLBC_OK;
}

PyObject *pyllbc_PackLemma_Dict::Read(pyllbc_Stream *stream)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("dict-lemma not done for unpack data");
        return NULL;
    }

    int len;
    LLBC_Stream &llbcStream = stream->GetLLBCStream();
    if (!llbcStream.ReadSInt32(len))
    {
        pyllbc_SetError("not enough bytes to unpack dict data(head area)");
        return NULL;
    }

    if (UNLIKELY(len < 0))
    {
        pyllbc_SetError("when unpacking dict data, unpack len < 0");
        return NULL;
    }

    PyObject *dict = PyDict_New();
    for (int i = 0; i < len; i++)
    {
        PyObject *key = _keyLemma->Read(stream);
        if (!key)
        {
            Py_DECREF(dict);
            return NULL;
        }

        PyObject *value = _valueLemma->Read(stream);
        if (!value)
        {
            Py_DECREF(key);
            Py_DECREF(dict);

            return NULL;
        }

        if (PyDict_SetItem(dict, key, value) != 0)
        {
            Py_DECREF(value);
            Py_DECREF(key);

            Py_DECREF(dict);

            pyllbc_TransferPyError("when unpack dict data from stream");
            return NULL;
        }

        Py_DECREF(value);
        Py_DECREF(key);
    }

    return dict;
}

int pyllbc_PackLemma_Dict::Write(pyllbc_Stream *stream, PyObject *values)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("could not pack not done dict-lemma");
        return LLBC_FAILED;
    }

    if (!PyDict_Check(values))
    {
        pyllbc_SetError("dict-lemma could not pack non-dict type object");
        return LLBC_FAILED;
    }

    const Py_ssize_t len = PyDict_Size(values);
    LLBC_Stream &llbcStream = stream->GetLLBCStream();

#if _M_X64
    llbcStream.WriteSInt32(static_cast<sint32>(len));
#else
    llbcStream.WriteSInt32(len);
#endif

    Py_ssize_t pos = 0;
    PyObject *key, *value;
    while (PyDict_Next(values, &pos, &key, &value))
    {
        if (_keyLemma->Write(stream, key) != LLBC_OK)
            return LLBC_FAILED;
        if (_valueLemma->Write(stream, value) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}
