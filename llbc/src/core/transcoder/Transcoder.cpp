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


#include "llbc/common/Export.h"

// iconv lib header file.
#if LLBC_TARGET_PLATFORM_WIN32
 #include "llbc/core/iconv/iconv.h"
#else // Non-Win32
 #include <iconv.h>
#endif // Win32

#include "llbc/core/file/File.h"

#include "llbc/core/transcoder/Transcoder.h"

__LLBC_NS_BEGIN

LLBC_Transcoder::LLBC_Transcoder()
{
}

LLBC_Transcoder::~LLBC_Transcoder()
{
}

int LLBC_Transcoder::MultiByteToWideChar(const LLBC_String &fromCode, const LLBC_String &src, LLBC_WString &dest)
{
    if (fromCode.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    dest.clear();
    if (src.empty())
    {
        return LLBC_OK;
    }

    iconv_t cd = iconv_open("UTF-16LE", fromCode.c_str());
    if (cd == reinterpret_cast<iconv_t>(-1))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    size_t inLen = src.length();
    char *in = const_cast<char *>(src.c_str());

    size_t outLen = (inLen + 1) * sizeof(wchar);
    dest.resize(outLen / sizeof(wchar));
    char *out = const_cast<char *>(reinterpret_cast<const char *>(dest.c_str()));

    if (iconv(cd, &in, &inLen, &out, &outLen) == (size_t)(-1))
    {
        dest.clear();
        iconv_close(cd);

        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    iconv_close(cd);
    dest.resize((out - reinterpret_cast<const char *>(dest.c_str())) / sizeof(wchar));

    return LLBC_OK;
}

int LLBC_Transcoder::MultiByteFileToWideChar(const LLBC_String &fromCode, const LLBC_String &srcFile, LLBC_WString &dest)
{
    LLBC_String srcContent;
    if (ReadMultiByteFile(srcFile, srcContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return MultiByteToWideChar(fromCode, srcContent, dest);
}

int LLBC_Transcoder::MultiByteToWideCharFile(const LLBC_String &fromCode, const LLBC_String &src, const LLBC_String &destFile)
{
    LLBC_WString destContent;
    if (MultiByteToWideChar(fromCode, src, destContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WriteWideCharToFile(destContent, destFile);
}

int LLBC_Transcoder::MultiByteFileToWideCharFile(const LLBC_String &fromCode, const LLBC_String &srcFile, const LLBC_String &destFile)
{
    LLBC_WString destContent;
    if (MultiByteFileToWideChar(fromCode, srcFile, destContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WriteWideCharToFile(destContent, destFile);
}

int LLBC_Transcoder::WideCharToMultiByte(const LLBC_String &toCode, const LLBC_WString &src, LLBC_String &dest)
{
    if (toCode.empty())
    {
        return LLBC_FAILED;
    }

    dest.clear();
    if (src.empty())
    {
        return LLBC_OK;
    }

    iconv_t cd = iconv_open(toCode.c_str(), "UTF-16LE");
    if (cd == (iconv_t)(-1))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    size_t inLen = src.length() * sizeof(wchar);
    char *in = reinterpret_cast<char *>(const_cast<wchar *>(src.c_str()));

    size_t outLen = inLen * 2 + 4;
    dest.resize(outLen);
    char *out = const_cast<char *>(dest.c_str());

    if (iconv(cd, &in, &inLen, &out, &outLen) == (size_t)(-1))
    {
        dest.clear();
        iconv_close(cd);

        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    iconv_close(cd);
    dest.resize(out - dest.c_str());

    return LLBC_OK;
}

int LLBC_Transcoder::WideCharFileToMultiByte(const LLBC_String &toCode, const LLBC_String &srcFile, LLBC_String &dest)
{
    LLBC_WString srcContent;
    if (ReadWideCharFile(srcFile, srcContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WideCharToMultiByte(toCode, srcContent, dest);
}

int LLBC_Transcoder::WideCharToMultiByteFile(const LLBC_String &toCode, const LLBC_WString &src, const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (WideCharToMultiByte(toCode, src, destContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WriteMultiByteToFile(destContent, destFile);
}

int LLBC_Transcoder::WideCharFileToMultiByteFile(const LLBC_String &toCode, const LLBC_String &srcFile, const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (WideCharFileToMultiByte(toCode, srcFile, destContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WriteMultiByteToFile(destContent, destFile);
}

int LLBC_Transcoder::MultiByteToMultiByte(const LLBC_String &fromCode,
                                          const LLBC_String &src,
                                          const LLBC_String &toCode,
                                          LLBC_String &dest)
{
    if (fromCode == toCode)
    {
        dest.clear();
        dest.append(src.c_str(), src.length());

        return LLBC_OK;
    }

    LLBC_WString wStr;
    if (MultiByteToWideChar(fromCode, src, wStr) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WideCharToMultiByte(toCode, wStr, dest);
}

int LLBC_Transcoder::MultiByteFileToMultiByte(const LLBC_String &fromCode,
                                              const LLBC_String &srcFile,
                                              const LLBC_String &toCode,
                                              LLBC_String &dest)
{
    LLBC_String srcContent;
    if (ReadMultiByteFile(srcFile, srcContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return MultiByteToMultiByte(fromCode, srcContent, toCode, dest);
}

int LLBC_Transcoder::MultiByteToMultiByteFile(const LLBC_String &fromCode,
                                              const LLBC_String &src,
                                              const LLBC_String &toCode,
                                              const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (MultiByteToMultiByte(fromCode, src, toCode, destContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WriteMultiByteToFile(destContent, destFile);
}

int LLBC_Transcoder::MultiByteFileToMultiByteFile(const LLBC_String &fromCode, const LLBC_String &srcFile, const LLBC_String &toCode, const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (MultiByteFileToMultiByte(fromCode, srcFile, toCode, destContent) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    return WriteMultiByteToFile(destContent, destFile);
}

LLBC_WString LLBC_Transcoder::MultiByteToWideChar(const LLBC_String &fromCode, const LLBC_String &src)
{
    LLBC_WString dest;
    if (MultiByteToWideChar(fromCode, src, dest) == LLBC_OK)
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return dest;
}

LLBC_String LLBC_Transcoder::WideCharToMultiByte(const LLBC_String &toCode, const LLBC_WString &src)
{
    LLBC_String dest;
    if (WideCharToMultiByte(toCode, src, dest) == LLBC_OK)
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return dest;
}

LLBC_String LLBC_Transcoder::MultiByteToMultiByte(const LLBC_String &fromCode, const LLBC_String &src, const LLBC_String &toCode)
{
    LLBC_String dest;
    if (MultiByteToMultiByte(fromCode, src, toCode, dest) == LLBC_OK)
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return dest;
}

    int LLBC_Transcoder::ReadMultiByteFile(const LLBC_String &fileName, LLBC_String &content)
{
    content = LLBC_File::ReadToEnd(fileName);
    if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
        return LLBC_OK;
    else
        return LLBC_FAILED;
}

int LLBC_Transcoder::ReadWideCharFile(const LLBC_String &fileName, LLBC_WString &content)
{
    LLBC_File file(fileName, LLBC_FileMode::BinaryRead);
    if (!file.IsOpened())
        return LLBC_FAILED;

    const long contentSize = file.GetFileSize();
    if (UNLIKELY(contentSize < 0))
        return LLBC_FAILED;

    if (contentSize % 2 != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }

    content.resize(contentSize / 2);
    const long actuallyRead = file.Read(const_cast<wchar *>(content.c_str()), contentSize);
    if (actuallyRead == -1)
    {
        content.resize(0);
        return LLBC_FAILED;
    }
    else if (actuallyRead != contentSize)
    {
        content.resize(0);

        LLBC_SetLastError(LLBC_ERROR_TRUNCATED);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_Transcoder::WriteMultiByteToFile(const LLBC_String &content, const LLBC_String &fileName)
{
    LLBC_File file(fileName, LLBC_FileMode::BinaryWrite);
    if (!file.IsOpened())
        return LLBC_FAILED;

    return file.Write(content);
}

int LLBC_Transcoder::WriteWideCharToFile(const LLBC_WString &content, const LLBC_String &fileName)
{
    LLBC_File file(fileName, LLBC_FileMode::BinaryWrite);
    if (!file.IsOpened())
        return LLBC_FAILED;

    return file.Write(content);
}

__LLBC_NS_END
