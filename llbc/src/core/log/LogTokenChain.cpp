/**
 * @file    LogTokenChain.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/ILogToken.h"
#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogTokenBuilder.h"
#include "llbc/core/log/LogTokenChain.h"

__LLBC_INTERNAL_NS_BEGIN

/**
 * Some token parse state constant values define.
 */
static const int __g_literal_state = 0;
static const int __g_converter_state = 1;
static const int __g_number_state = 2;

/**
 * Default pattern.
 */
static const char *__g_default_pattern = "%T [%L] - %m%n";

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogTokenChain::LLBC_LogTokenChain()
: _head(NULL)
{
}

LLBC_LogTokenChain::~LLBC_LogTokenChain()
{
    this->Cleanup();
}

int LLBC_LogTokenChain::Build(const LLBC_String &pattern)
{
    if (_head)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    char ch = '\0';
    const char *curPattern = NULL;
    LLBC_String::size_type patternLength = 0;
    int state = LLBC_INTERNAL_NS __g_literal_state;

    LLBC_ILogToken *token = NULL;
    LLBC_LogFormattingInfo *formatter = NULL;

    LLBC_String buf;
    if (pattern.empty())
    {
        curPattern = LLBC_INTERNAL_NS __g_default_pattern;
        patternLength = LLBC_StrLenA(LLBC_INTERNAL_NS __g_default_pattern);
    }
    else
    {
        curPattern = pattern.data();
        patternLength = pattern.size();
    }

    for (size_t i = 0; i < patternLength;)
    {
        ch = curPattern[i ++];
        switch(state)
        {
        case LLBC_INTERNAL_NS __g_literal_state:
            if (i == patternLength)
            {
                buf.append(1, ch);
                break;
            }

            if (ch == LLBC_LogTokenType::EscapeToken)
            {
                if (curPattern[i] == LLBC_LogTokenType::EscapeToken)
                {
                    buf.append(1, ch);
                    i ++;
                }
                else
                {
                    if (!buf.empty())
                    {
                        token = LLBC_LogTokenBuilderSingleton->BuildLogToken(LLBC_LogTokenType::StrToken);
                        token->Initialize(formatter, buf);
                        this->AppendToken(token);

                        buf.clear();
                        formatter = NULL;
                    }

                    buf.append(1, ch);
                    LLBC_XDelete(formatter);
                    state = LLBC_INTERNAL_NS __g_converter_state;
                }
            }
            else
            {
                buf.append(1, ch);
            }

            break;

        case LLBC_INTERNAL_NS __g_converter_state:
            buf.append(1, ch);
            
            if ((ch >= 0x30 && ch <= 0x39) || ch == '-')
            {
                state = LLBC_INTERNAL_NS __g_number_state;
                break;
            }

            buf.erase(buf.rfind(LLBC_LogTokenType::EscapeToken));
            token = LLBC_LogTokenBuilderSingleton->BuildLogToken(ch);
            if (!formatter)
            {
                formatter = new LLBC_LogFormattingInfo;
            }

            token->Initialize(formatter, "");
            this->AppendToken(token);

            formatter = NULL;
            state = LLBC_INTERNAL_NS __g_literal_state;

            break;

        case LLBC_INTERNAL_NS __g_number_state:
            if ((ch < 0x30 || ch > 0x39) && ch != '-')
            {
                int minLength = LLBC_Str2Int32(&buf[buf.rfind(LLBC_LogTokenType::EscapeToken) + 1]);
                if (!formatter)
                {
                    formatter = new LLBC_LogFormattingInfo(minLength < 0 ? true : false, ::abs(minLength), INT_MAX);
                }
                else
                {
                    formatter->SetLeftAligh(minLength < 0 ? true : false);
                    formatter->SetMinLen(minLength);
                    formatter->SetMaxLen(INT_MAX);
                }

                i --;
                state = LLBC_INTERNAL_NS __g_converter_state;

                break;
            }

            buf.append(1, ch);
            break;

        default:
            LLBC_XDelete(formatter);
            this->Cleanup();

            LLBC_SetLastError(LLBC_ERROR_FORMAT);
            return LLBC_FAILED;

            break;
        }
    }

    if (!buf.empty())
    {
        token = LLBC_LogTokenBuilderSingleton->BuildLogToken(LLBC_LogTokenType::StrToken);
        token->Initialize(NULL, buf);
        this->AppendToken(token);
    }

    return LLBC_OK;
}

void LLBC_LogTokenChain::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    LLBC_ILogToken *token = _head;
    while (token)
    {
        token->Format(data, formattedData);
        token = token->GetTokenNext();
    }
}

void LLBC_LogTokenChain::Cleanup()
{
    while (_head)
    {
        LLBC_ILogToken *next = _head->GetTokenNext();

        delete _head;
        _head = next;
    }
}

void LLBC_LogTokenChain::AppendToken(LLBC_ILogToken *token)
{
    token->SetTokenNext(NULL);

    if (!_head)
    {
        _head = token;
        return;
    }

    if (!_head->GetTokenNext())
    {
        _head->SetTokenNext(token);
        return;
    }

    LLBC_ILogToken *tmp = _head;
    while (tmp->GetTokenNext())
    {
        tmp= tmp->GetTokenNext();
    }

    tmp->SetTokenNext(token);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
