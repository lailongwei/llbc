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

namespace
{
    typedef pyllbc_PackLemma Base;

    typedef Base::Type Type;
    typedef Base::State State;
    typedef Base::Symbol Symbol;
}

pyllbc_PackLemma_Raw::pyllbc_PackLemma_Raw(PyObject *compileEnv)
: Base(compileEnv)
, _symbol(Base::InvalidSymbol)
, _str2Flag(false)
{
}

pyllbc_PackLemma_Raw::~pyllbc_PackLemma_Raw()
{
}

Type pyllbc_PackLemma_Raw::GetType() const
{
    return Base::RawType;
}

bool pyllbc_PackLemma_Raw::IsSerializable() const
{
    return true;
}

int pyllbc_PackLemma_Raw::Process(Symbol ch, Symbol nextCh)
{
    if (UNLIKELY(IsErrorOccurred()))
    {
        return LLBC_FAILED;
    }
    else if (UNLIKELY(IsDone()))
    {
        _state = Base::Error;
        pyllbc_SetError("raw-lemma is done, could not continue to accept any other lemmas");

        return LLBC_FAILED;
    }
    else if (_state == Base::Accepting)
    {
        if (UNLIKELY(ch != Base::StringLen))
        {
            _state = Base::Error;

            LLBC_String errStr;
            pyllbc_SetError(errStr.format("str type raw-lemma except '#' lemma to done, but got: %c", ch));

            return LLBC_FAILED;
        }

        _str2Flag = true;
        _state = Done;

        return LLBC_OK;
    }

    const SymbolGroup &raw = GroupedSymbol::Raw();
    if (raw.find(ch) == raw.end())
    {
        _state = Base::Error;

        LLBC_String errStr;
        pyllbc_SetError(errStr.format("invalid raw-lemma character: %c", ch));

        return LLBC_FAILED;
    }
    else if (ch == Base::StringLen) // Special raw-lemma character, process it.
    {
        _state = Base::Error;

        pyllbc_SetError("str len lemma '#' must place on str lemma 'S' later");
        return LLBC_FAILED;
    }

    _symbol = ch;
    _str.append(1, static_cast<char>(ch));

    _state = (_symbol == Base::String && nextCh == Base::StringLen) ? Base::Accepting : Base::Done;

    return LLBC_OK;
}

int pyllbc_PackLemma_Raw::Process(Base *lemma)
{
    _state = Base::Error;
    pyllbc_SetError("raw-lemma not accept any non-raw lamma");

    return LLBC_FAILED;
}

PyObject *pyllbc_PackLemma_Raw::Read(pyllbc_Stream *stream)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("raw-lemma not done to unpack data");
        return nullptr;
    }

    PyObject *obj = nullptr;
    switch(_symbol)
    {
    case Base::Byte:
        obj = stream->ReadByte();
        break;
    
    case Base::Boolean:
        obj = stream->ReadBool();
        break;

    case Base::Int16:
        obj = stream->ReadInt16();
        break;

    case Base::Int32:
        obj = stream->ReadInt32();
        break;

    case Base::Int64:
        obj = stream->ReadInt64();
        break;

    case Base::Float:
        obj = stream->ReadFloat();
        break;

    case Base::Double:
        obj = stream->ReadDouble();
        break;

    case Base::String:
        if (_str2Flag)
            obj = stream->ReadStr2();
        else
            obj = stream->ReadStr();
        break;

    case Base::Unicode:
        obj = stream->ReadUnicode();
        break;

    case Base::ByteArray:
        obj = stream->ReadByteArray();
        break;

    case Base::Buffer:
        obj = stream->ReadBuffer();
        break;

    case Base::None:
        obj = stream->ReadNone();
        break;

    default:
        obj = nullptr;
        {
            LLBC_String errStr;
            pyllbc_SetError(
                errStr.format("Unknown raw-lemma symbol: %c", _symbol));
        }
        break;
    }

    return obj;
}

int pyllbc_PackLemma_Raw::Write(pyllbc_Stream *stream, PyObject *values)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("raw-lemma not done to pack data");
        return LLBC_FAILED;
    }

    int rtn;
    switch(_symbol)
    {
    case Base::Byte:
        rtn = stream->WriteByte(values);
        break;

    case Base::Boolean:
        rtn = stream->WriteBool(values);
        break;

    case Base::Int16:
        rtn = stream->WriteInt16(values);
        break;

    case Base::Int32:
        rtn = stream->WriteInt32(values);
        break;

    case Base::Int64:
        rtn = stream->WriteInt64(values);
        break;

    case Base::Float:
        rtn = stream->WriteFloat(values);
        break;

    case Base::Double:
        rtn = stream->WriteDouble(values);
        break;

    case Base::String:
        if (_str2Flag)
            rtn = stream->WriteStr2(values);
        else
            rtn = stream->WriteStr(values);
        break;

    case Base::Unicode:
        rtn = stream->WriteUnicode(values);
        break;

    case Base::ByteArray:
        rtn = stream->WriteByteArray(values);
        break;

    case Base::Buffer:
        rtn = stream->WriteBuffer(values);
        break;

    case Base::None:
        rtn = stream->WriteNone(values);
        break;

    default:
        rtn = LLBC_FAILED;
        {
            LLBC_String errStr;
            pyllbc_SetError(errStr.format(
                "unknown format character in raw-lemma: %c", _symbol));
        }
        break;
    }

    return rtn;
}
