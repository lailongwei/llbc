/**
 * @file    PackLemma.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/01
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/PackLemma.h"

namespace
{
    typedef pyllbc_PackLemma This;

    typedef This::Type Type;
    typedef This::State State;
    typedef This::Symbol Symbol;
    typedef This::SymbolGroup SymbolGroup;
}

SymbolGroup This::GroupedSymbol::_raw;
SymbolGroup This::GroupedSymbol::_seqBegin;
SymbolGroup This::GroupedSymbol::_seqEnd;
SymbolGroup This::GroupedSymbol::_listPair;
SymbolGroup This::GroupedSymbol::_tuplePair;

const SymbolGroup &This::GroupedSymbol::Raw()
{
    if (UNLIKELY(_raw.empty()))
    {
        _raw.insert(This::Byte);
        _raw.insert(This::Boolean);
        _raw.insert(This::Int16);
        _raw.insert(This::Int32);
        _raw.insert(This::Int64);
        _raw.insert(This::Float);
        _raw.insert(This::Double);
        _raw.insert(This::String);
        _raw.insert(This::StringLen);
        _raw.insert(This::Unicode);
        _raw.insert(This::ByteArray);
        _raw.insert(This::Buffer);
        _raw.insert(This::None);
    }

    return _raw;
}

const SymbolGroup &This::GroupedSymbol::SeqBegin()
{
    if (UNLIKELY(_seqBegin.empty()))
    {
        _seqBegin.insert(This::ListBegin);
        _seqBegin.insert(This::TupleBegin);
    }

    return _seqBegin;
}

const SymbolGroup &This::GroupedSymbol::SeqEnd()
{
    if (UNLIKELY(_seqEnd.empty()))
    {
        _seqEnd.insert(This::ListEnd);
        _seqEnd.insert(This::TupleEnd);
    }

    return _seqEnd;
}

const SymbolGroup &This::GroupedSymbol::ListPair()
{
    if (UNLIKELY(_listPair.empty()))
    {
        _listPair.insert(This::ListBegin);
        _listPair.insert(This::ListEnd);
    }

    return _listPair;
}

const SymbolGroup &This::GroupedSymbol::TuplePair()
{
    if (UNLIKELY(_tuplePair.empty()))
    {
        _tuplePair.insert(This::TupleBegin);
        _tuplePair.insert(This::TupleEnd);
    }

    return _tuplePair;
}

pyllbc_PackLemma::pyllbc_PackLemma(PyObject *compileEnv)
: _state(This::Begin)
, _str()
, _emptyStr()

, _compileEnv(compileEnv)
{
}

pyllbc_PackLemma::~pyllbc_PackLemma()
{
}

State pyllbc_PackLemma::GetState() const
{
    return _state;
}

bool pyllbc_PackLemma::IsDone() const
{
    return _state == This::Done;
}

bool pyllbc_PackLemma::IsErrorOccurred() const
{
    return _state == This::Error;
}

const LLBC_String &pyllbc_PackLemma::ToString() const
{
    if (!IsDone())
        return _emptyStr;

    return _str;
}
