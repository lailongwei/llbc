/**
 * @file    Transcoder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/16
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/file/File.h"
#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/iconv/iconv.h"
#endif // LLBC_TARGET_PLATFORM_WIN32

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
        return LLBC_RTN_FAILED;
    }

    dest.clear();
    if (src.empty())
    {
        return LLBC_RTN_OK;
    }

    iconv_t cd = iconv_open("UTF-16LE", fromCode.c_str());
    if (cd == reinterpret_cast<iconv_t>(-1))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
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
        return LLBC_RTN_FAILED;
    }

    iconv_close(cd);
    dest.resize((out - reinterpret_cast<const char *>(dest.c_str())) / sizeof(wchar));

    return LLBC_RTN_OK;
}

int LLBC_Transcoder::MultiByteFileToWideChar(const LLBC_String &fromCode, const LLBC_String &srcFile, LLBC_WString &dest)
{
    LLBC_String srcContent;
    if (this->ReadMultiByteFile(srcFile, srcContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->MultiByteToWideChar(fromCode, srcContent, dest);
}

int LLBC_Transcoder::MultiByteToWideCharFile(const LLBC_String &fromCode, const LLBC_String &src, const LLBC_String &destFile)
{
    LLBC_WString destContent;
    if (this->MultiByteToWideChar(fromCode, src, destContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WriteWideCharToFile(destContent, destFile);
}

int LLBC_Transcoder::MultiByteFileToWideCharFile(const LLBC_String &fromCode, const LLBC_String &srcFile, const LLBC_String &destFile)
{
    LLBC_WString destContent;
    if (this->MultiByteFileToWideChar(fromCode, srcFile, destContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WriteWideCharToFile(destContent, destFile);
}

int LLBC_Transcoder::WideCharToMultiByte(const LLBC_String &toCode, const LLBC_WString &src, LLBC_String &dest)
{
    if (toCode.empty())
    {
        return LLBC_RTN_FAILED;
    }

    dest.clear();
    if (src.empty())
    {
        return LLBC_RTN_OK;
    }

    iconv_t cd = iconv_open(toCode.c_str(), "UTF-16LE");
    if (cd == (iconv_t)(-1))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
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
        return LLBC_RTN_FAILED;
    }

    iconv_close(cd);
    dest.resize(out - dest.c_str());

    return LLBC_RTN_OK;
}

int LLBC_Transcoder::WideCharFileToMultiByte(const LLBC_String &toCode, const LLBC_String &srcFile, LLBC_String &dest)
{
    LLBC_WString srcContent;
    if (this->ReadWideCharFile(srcFile, srcContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WideCharToMultiByte(toCode, srcContent, dest);
}

int LLBC_Transcoder::WideCharToMultiByteFile(const LLBC_String &toCode, const LLBC_WString &src, const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (this->WideCharToMultiByte(toCode, src, destContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WriteMultiByteToFile(destContent, destFile);
}

int LLBC_Transcoder::WideCharFileToMultiByteFile(const LLBC_String &toCode, const LLBC_String &srcFile, const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (this->WideCharFileToMultiByte(toCode, srcFile, destContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WriteMultiByteToFile(destContent, destFile);
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

        return LLBC_RTN_OK;
    }

    LLBC_WString wStr;
    if (this->MultiByteToWideChar(fromCode, src, wStr) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WideCharToMultiByte(toCode, wStr, dest);
}

int LLBC_Transcoder::MultiByteFileToMultiByte(const LLBC_String &fromCode,
                                              const LLBC_String &srcFile,
                                              const LLBC_String &toCode,
                                              LLBC_String &dest)
{
    LLBC_String srcContent;
    if (this->ReadMultiByteFile(srcFile, srcContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->MultiByteToMultiByte(fromCode, srcContent, toCode, dest);
}

int LLBC_Transcoder::MultiByteToMultiByteFile(const LLBC_String &fromCode,
                                              const LLBC_String &src,
                                              const LLBC_String &toCode,
                                              const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (this->MultiByteToMultiByte(fromCode, src, toCode, destContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WriteMultiByteToFile(destContent, destFile);
}

int LLBC_Transcoder::MultiByteFileToMultiByteFile(const LLBC_String &fromCode, const LLBC_String &srcFile, const LLBC_String &toCode, const LLBC_String &destFile)
{
    LLBC_String destContent;
    if (this->MultiByteFileToMultiByte(fromCode, srcFile, toCode, destContent) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    return this->WriteMultiByteToFile(destContent, destFile);
}

int LLBC_Transcoder::ReadMultiByteFile(const LLBC_String &fileName, LLBC_String &content)
{
    LLBC_File file;
    if (file.Open(fileName, "rb") != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    const size_t contentSize = file.GetSize();
    if (UNLIKELY(contentSize < 0))
    {
        return LLBC_RTN_FAILED;
    }

    content.resize(contentSize);
    if (file.Read(const_cast<char *>(content.c_str()), contentSize) != contentSize)
    {
        content.clear();
        return LLBC_RTN_FAILED;
    }

    return file.Close();
}

int LLBC_Transcoder::ReadWideCharFile(const LLBC_String &fileName, LLBC_WString &content)
{
    LLBC_File file;
    if (file.Open(fileName, "rb") != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    const size_t contentSize = file.GetSize();
    if (UNLIKELY(contentSize < 0))
    {
        return LLBC_RTN_FAILED;
    }

    if (contentSize % 2 != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_RTN_FAILED;
    }

    content.resize(contentSize / 2);
    if (file.Read(const_cast<wchar *>(content.c_str()), contentSize) != contentSize)
    {
        content.clear();
        return LLBC_RTN_FAILED;
    }

    return file.Close();
}

int LLBC_Transcoder::WriteMultiByteToFile(const LLBC_String &content, const LLBC_String &fileName)
{
    LLBC_File file;
    if (file.Open(fileName, "wb") != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    if (content.empty())
    {
        return LLBC_RTN_OK;
    }

    const size_t cntLen = content.size();
    if (file.Write(content.c_str(), cntLen) != cntLen)
    {
        return LLBC_RTN_FAILED;
    }

    return file.Close();
}

int LLBC_Transcoder::WriteWideCharToFile(const LLBC_WString &content, const LLBC_String &fileName)
{
    LLBC_File file;
    if (file.Open(fileName, "wb") != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    const size_t contentSize = content.size() * sizeof(wchar);
    if (contentSize == 0)
    {
        return LLBC_RTN_OK;
    }

    if (file.Write(content.c_str(), contentSize) != contentSize)
    {
        return LLBC_RTN_FAILED;
    }

    return file.Close();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
