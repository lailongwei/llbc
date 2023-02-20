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

inline int pyllbc_Stream::GetEndian() const
{
    return _stream.GetEndian();
}

inline int pyllbc_Stream::SetEndian(int endian)
{
    if (!LLBC_Endian::IsValid(endian))
    {
        pyllbc_SetError("Invalid endian value", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _stream.SetEndian(endian);

    return LLBC_OK;
}

inline size_t pyllbc_Stream::GetPos() const
{
    return _stream.GetPos();
}

inline int pyllbc_Stream::SetPos(size_t pos)
{
    if (pos > _stream.GetCap())
    {
        pyllbc_SetError("pos out of range", LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    _stream.SetPos(pos);

    return LLBC_OK;
}

inline size_t pyllbc_Stream::GetCap() const
{
    return _stream.GetCap();
}

inline int pyllbc_Stream::Recap(size_t newCap)
{
    if (_stream.IsAttach())
    {
        pyllbc_SetError("could not set attach buffer stream newCap"
                        "(attached buffer maybe from pyllbc native library or other native libraries)",
                        LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    _stream.Recap(newCap);

    return LLBC_OK;
}

inline LLBC_Stream &pyllbc_Stream::GetLLBCStream()
{
    return _stream;
}

inline const LLBC_Stream &pyllbc_Stream::GetLLBCStream() const
{
    return _stream;
}

inline PyObject *pyllbc_Stream::GetPyObj()
{
    Py_INCREF(_pyStream);
    return _pyStream;
}


