// The MIT License(MIT)

// Copyright(c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files(the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "pyllbc/common/Errors.h"

LLBC_FORCE_INLINE int pyllbc_Stream::GetEndian() const
{
    return _stream.GetEndian();
}

LLBC_FORCE_INLINE int pyllbc_Stream::SetEndian(int endian)
{
    if (UNLIKELY(!LLBC_Endian::IsValid(endian)))
    {
        pyllbc_SetError("Invalid endian value", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _stream.SetEndian(endian);

    return LLBC_OK;
}

LLBC_FORCE_INLINE size_t pyllbc_Stream::GetReadPos() const
{
    return _stream.GetReadPos();
}

LLBC_FORCE_INLINE int pyllbc_Stream::SetReadPos(size_t readPos)
{
    if (!_stream.SetReadPos(readPos))
    {
        pyllbc_SetError("rpos out of range", LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

LLBC_FORCE_INLINE size_t pyllbc_Stream::GetWritePos() const
{
    return _stream.GetWritePos();
}

LLBC_FORCE_INLINE int pyllbc_Stream::SetWritePos(size_t writePos)
{
    if (!_stream.SetWritePos(writePos))
    {
        pyllbc_SetError("wpos out of range", LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

LLBC_FORCE_INLINE size_t pyllbc_Stream::GetCap() const
{
    return _stream.GetCap();
}

LLBC_FORCE_INLINE int pyllbc_Stream::Recap(size_t newCap)
{
    if (_stream.IsAttach())
    {
        pyllbc_SetError("could not recap in attach buffer stream", LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }
    else if (newCap < _stream.GetCap())
    {
        pyllbc_SetError("stream capacity cannot be reduced", LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    _stream.Recap(newCap);

    return LLBC_OK;
}

LLBC_FORCE_INLINE size_t pyllbc_Stream::GetReadableSize() const
{
    return _stream.GetReadableSize();
}

LLBC_FORCE_INLINE size_t pyllbc_Stream::GetWritableSize() const
{
    return _stream.GetWritableSize();
}

LLBC_FORCE_INLINE LLBC_Stream &pyllbc_Stream::GetLLBCStream()
{
    return _stream;
}

LLBC_FORCE_INLINE const LLBC_Stream &pyllbc_Stream::GetLLBCStream() const
{
    return _stream;
}

LLBC_FORCE_INLINE PyObject *pyllbc_Stream::GetPyObj()
{
    Py_INCREF(_pyStream);
    return _pyStream;
}


