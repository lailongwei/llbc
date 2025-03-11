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

#pragma once

#include "llbc/core/singleton/Singleton.h"

__LLBC_NS_BEGIN

/**
 * \brief The llbc library transcoder class encapsulation.
 */
class LLBC_EXPORT LLBC_Transcoder
{
private:
    LLBC_Transcoder();
    virtual ~LLBC_Transcoder();

public:
    /**
     * Map a character string to a wide-character(Unicode) string.
     * @param[in]  fromCode - from code page, like UTF8, GP936, BIG5, ... eg.
     * @param[in]  src      - string to map.
     * @param[out] dest     - wide-character string.
     * @param[in]  srcFile  - contain string file to map.
     * @param[out] destFile - wide-character string file.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int MultiByteToWideChar(const LLBC_String &fromCode, const LLBC_String &src, LLBC_WString &dest);
    static int MultiByteFileToWideChar(const LLBC_String &fromCode, const LLBC_String &srcFile, LLBC_WString &dest);
    static int MultiByteToWideCharFile(const LLBC_String &fromCode, const LLBC_String &src, const LLBC_String &destFile);
    static int MultiByteFileToWideCharFile(const LLBC_String &fromCode, const LLBC_String &srcFile, const LLBC_String &destFile);

    /**
     * Map a wide-character(Unicode) to a character string.
     * @param[in]  toCode   - to code page, like UTF8, GP936, BIG5, ... eg.
     * @param[in]  src      - string to map.
     * @param[in]  srcFile  - constin string file to map. 
     * @param[out] dest     - multi-byte character string.
     * @param[out] destFile - multi-byte character string file.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int WideCharToMultiByte(const LLBC_String &toCode, const LLBC_WString &src, LLBC_String &dest);
    static int WideCharFileToMultiByte(const LLBC_String &toCode, const LLBC_String &srcFile, LLBC_String &dest);
    static int WideCharToMultiByteFile(const LLBC_String &toCode, const LLBC_WString &src, const LLBC_String &destFile);
    static int WideCharFileToMultiByteFile(const LLBC_String &toCode, const LLBC_String &srcFile, const LLBC_String &destFile);

    /**
     * Map a multi-byte character string to another character-set's multi-byte character string.
     * @param[in]  fromCode - from code page.
     * @param[in]  src      - string to map.
     * @param[in]  srcFile  - contain string to map.
     * @param[in]  toCode   - another codepage name.
     * @param[out] dest     - multi-byte character string.
     * @param[out] destFile - multi-byte character string file.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int MultiByteToMultiByte(const LLBC_String &fromCode,
                                    const LLBC_String &src,
                                    const LLBC_String &toCode,
                                    LLBC_String &dest);
    static int MultiByteFileToMultiByte(const LLBC_String &fromCode,
                                        const LLBC_String &srcFile,
                                        const LLBC_String &toCode,
                                        LLBC_String &dest);
    static int MultiByteToMultiByteFile(const LLBC_String &fromCode,
                                        const LLBC_String &src,
                                        const LLBC_String &toCode,
                                        const LLBC_String &destFile);
    static int MultiByteFileToMultiByteFile(const LLBC_String &fromCode,
                                            const LLBC_String &srcFile,
                                            const LLBC_String &toCode,
                                            const LLBC_String &destFile);

    /**
     * Map a character string to a wide-character(Unicode) string.
     * @param[in] fromCode - from code page.
     * @param[in] src      - string to map.
     * @return LLBC_WString - the wide-character string.
     */
    static LLBC_WString MultiByteToWideChar(const LLBC_String &fromCode, const LLBC_String &src);

    /**
     * Map a wide-character(Unicode) to a character string.
     * @param[in] toCode - to code page, like UTF8, GP936, BIG5, ... eg.
     * @param[in] src    - string to map.
     * @return LLBC_String - the character string.
     */
    static LLBC_String WideCharToMultiByte(const LLBC_String &toCode, const LLBC_WString &src);

    /**
     * Map a multi-byte character string to another character-set's multi-byte character string.
     * @param[in]  fromCode - from code page.
     * @param[in]  src      - string to map.
     * @param[in]  toCode   - another codepage name.
     * @return LLBC_String - the another character-set's multi-byte character string.
     */
    static LLBC_String MultiByteToMultiByte(const LLBC_String &fromCode, const LLBC_String &src, const LLBC_String &toCode);

private:
    /**
     * Open multi-byte/wide-char file and read it's content.
     * @param[in]  fileName - file name.
     * @param[out] content  - file content.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int ReadMultiByteFile(const LLBC_String &fileName, LLBC_String &content);
    static int ReadWideCharFile(const LLBC_String &fileName, LLBC_WString &content);

    /**
     * Save multi-byte/wide-char content to file.
     * @param[in] content  - content.
     * @param[in] fileName - will save file name.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int WriteMultiByteToFile(const LLBC_String &content, const LLBC_String &fileName);
    static int WriteWideCharToFile(const LLBC_WString &content, const LLBC_String &fileName);
};

__LLBC_NS_END
