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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Log formatting information class encapsulation.
 */
class LLBC_EXPORT LLBC_LogFormattingInfo
{
public:
    /**
     * Parameter constructor, use to construct formatting info object.
     * @param[in] leftAlign     - specific message is left alignment or right alignment.
     * @param[in] minLen        - minimum log message length.
     * @param[in] maxLen        - maximum log message length.
     * @param[in] fillCharacter - fill character.
     */
    LLBC_LogFormattingInfo(bool leftAlign = false,
                           int minLen = 0,
                           int maxLen = INT_MAX,
                           int fillCharacter = ' ');

    ~LLBC_LogFormattingInfo();

public:
    /**
     * Get left align flag.
     * @return bool - left align flag.
     */
    bool GetLeftAlign() const;

    /**
     * Set left align flag.
     * @param[in] leftAlign - left align flag.
     */
    void SetLeftAligh(bool leftAligh);

    /**
     * Get minimum log message length.
     * @return int - minimum message length.
     */
    int GetMinLen() const;

    /**
     * Set minimum log message length.
     * @param[in] minLen - minimum message length.
     */
    void SetMinLen(int minLen);

    /**
     * Get maximum log message length.
     * @return int - maximum message length.
     */
    int GetMaxLen() const;

    /**
     * Set maximum log message length.
     * @param[in] maxLen - maximum message length.
     */
    void SetMaxLen(int maxLen);

    /**
     * Get fill character.
     * @return char - fill character.
     */
    char GetFillCharacter() const;

    /**
     * Set fill character.
     * @param[in] char - fill character.
     */
    void SetFillCharacter(char fillCharacter);

public:
    /**
     * Format the given data, start by field start index.
     * @param[in/out] data   - log data.
     * @param[in] fieldStart - begin formatting position.
     */
    void Format(LLBC_String &data, int fieldStart) const;

private:
    bool _leftAlign;

    int _minLen;
    int _maxLen;

    char _fillCharacter;
};

__LLBC_NS_END
