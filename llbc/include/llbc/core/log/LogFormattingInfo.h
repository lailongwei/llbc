/**
 * @file    LogFormattingInfo.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_FORMATTING_INFO_H__
#define __LLBC_CORE_LOG_LOG_FORMATTING_INFO_H__

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

#endif // !__LLBC_CORE_LOG_LOG_FORMATTING_INFO_H__
