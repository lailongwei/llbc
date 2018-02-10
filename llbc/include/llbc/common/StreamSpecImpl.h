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

#ifdef __LLBC_COM_STREAM_H__

__LLBC_NS_BEGIN

/* Adapted sint16/uint16/sint32/uint32/long/unsigned long/sint64/uint64/float/double types Read/Write template method */
template <>
inline bool LLBC_Stream::Read(sint16 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const sint16 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(uint16 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const uint16 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(sint32 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const sint32 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(uint32 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const uint32 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(long &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const long &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(unsigned long &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const unsigned long &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(sint64 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const sint64 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(uint64 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const uint64 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(float &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const float &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::Read(double &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::Write(const double &obj)
{
    this->WriteRawType(obj);
}

/* Adapted sint16/uint16/sint32/uint32/long/unsigned long/sint64/uint64/float/double types ReadEx/WriteEx template method */
template <>
inline bool LLBC_Stream::ReadEx(sint16 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const sint16 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(uint16 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const uint16 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(sint32 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const sint32 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(uint32 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const uint32 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(long &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const long &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(unsigned long &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const unsigned long &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(sint64 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const sint64 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(uint64 &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const uint64 &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(float &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const float &obj)
{
    this->WriteRawType(obj);
}

template <>
inline bool LLBC_Stream::ReadEx(double &obj)
{
    return this->ReadRawType(obj);
}

template <>
inline void LLBC_Stream::WriteEx(const double &obj)
{
    this->WriteRawType(obj);
}


/* Adapted std::string read/write readex/writeex. */
template <>
inline bool LLBC_Stream::Read(std::string &obj)
{
    obj.clear();

    if (this->GetSize() - this->GetPos() == 0)
        return false;

    char elem = char();
    while (this->Read(elem))
    {
        if (elem == '\0')
            break;

        obj.append(1, elem);
    }

    return true;
}

template <>
inline bool LLBC_Stream::ReadEx(std::string &obj)
{
    uint32 len = 0;
    if (!this->ReadEx(len))
        return false;

    obj.resize(len);
    return this->ReadBuffer(const_cast<char *>(obj.data()), obj.size());
}

template <>
inline void LLBC_Stream::Write(const std::string &obj)
{
    if (!obj.empty())
        this->WriteBuffer(obj.data(), obj.size());

    this->WriteBuffer("\0", 1);
}

template <>
inline void LLBC_Stream::WriteEx(const std::string &obj)
{
    this->WriteEx(static_cast<uint32>(obj.size()));
    this->WriteBuffer(obj.data(), obj.size());
}

__LLBC_NS_END

#endif // __LLBC_COM_STREAM_H__

