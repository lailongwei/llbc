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

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/BaseLogToken.h"
#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogTokenBuilder.h"
#include "llbc/core/log/LogTokenChain.h"

__LLBC_INTERNAL_NS_BEGIN

/**
 * Default pattern.
 */
static const char __g_default_pattern[] = "%T [%L] - %m%n";

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogTokenChain::LLBC_LogTokenChain()
: _head(nullptr)
{
}

LLBC_LogTokenChain::~LLBC_LogTokenChain()
{
    Cleanup();
}

int LLBC_LogTokenChain::Build(const LLBC_String &pattern)
{
    if (_head)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Define token parse state.
    enum class TokenParseState
    {
        NormalState, // Normal state.
        ParsingToken, // Parsing token state.
        ParsingTokenIndent, // Parsing token indent state.
        ParsingTokenAddiParam, // Parsing token additional params state.
    };

    // Get pattern and length.
    const char *curPattern;
    LLBC_String::size_type patternLen;
    if (pattern.empty())
    {
        curPattern = LLBC_INTERNAL_NS __g_default_pattern;
        patternLen = sizeof(LLBC_INTERNAL_NS __g_default_pattern) - 1;
    }
    else
    {
        curPattern = pattern.data();
        patternLen = pattern.size();
    }

    // Parse pattern.
    LLBC_String buf;
    LLBC_BaseLogToken *token = nullptr;
    LLBC_LogFormattingInfo formatter;
    auto state = TokenParseState::NormalState;
    auto tokenBuilder = LLBC_LogTokenBuilderSingleton;
    for (size_t i = 0; i < patternLen; )
    {
        const char ch = curPattern[i++];
        switch(state)
        {
        case TokenParseState::NormalState:
            if (i == patternLen)
            {
                buf.append(1, ch);
                break;
            }

            if (ch == LLBC_LogTokenType::EscapeToken)
            {
                if (curPattern[i] == LLBC_LogTokenType::EscapeToken)
                {
                    buf.append(1, ch);
                    i++;
                }
                else
                {
                    if (!buf.empty())
                    {
                        token = tokenBuilder->BuildLogToken(LLBC_LogTokenType::StrToken);
                        token->Initialize(formatter, buf);
                        AppendToken(token);

                        buf.clear();
                    }

                    buf.append(1, ch);
                    formatter.Reset();
                    state = TokenParseState::ParsingToken;
                }
            }
            else
            {
                buf.append(1, ch);
            }

            break;

        case TokenParseState::ParsingToken:
            buf.append(1, ch);

            // Try convert to <ParsingTokenIndent> state.
            if ((ch >= '0' && ch <= '9') || ch == '-')
            {
                state = TokenParseState::ParsingTokenIndent;
                break;
            }

            // Try convert to <ParsingTokenAddiParam> state.
            if (pattern[i] == '{')
            {
                state = TokenParseState::ParsingTokenAddiParam;
                break;
            }

            buf.erase(buf.rfind(LLBC_LogTokenType::EscapeToken));
            token = tokenBuilder->BuildLogToken(ch);

            token->Initialize(formatter, "");
            AppendToken(token);

            formatter.Reset();
            state = TokenParseState::NormalState;

            break;

        case TokenParseState::ParsingTokenIndent:
            if ((ch < '0' || ch > '9') && ch != '-')
            {
                const int minLength = LLBC_Str2Num<int>(&buf[buf.rfind(LLBC_LogTokenType::EscapeToken) + 1]);
                formatter.leftAlign = minLength < 0 ? true : false;
                formatter.minLen = std::abs(minLength);

                i--;
                state = TokenParseState::ParsingToken;

                break;
            }

            buf.append(1, ch);
            break;

        case TokenParseState::ParsingTokenAddiParam:
            if (ch == '}')
            {
                // Get token additional param.
                formatter.addiParam = buf.substr(buf.rfind("{") + 1);

                // Get token define begin position.
                const size_t tokenBeginPos = buf.rfind(LLBC_LogTokenType::EscapeToken);

                // Get token position(skip indent prefix setting).
                size_t tokenPos = tokenBeginPos + 1;
                while (tokenPos < buf.size() - 1 &&
                       (buf[tokenPos] == '-' || (buf[tokenPos] >= '0' && buf[tokenPos] <= '9')))
                    ++tokenPos;

                // Build token.
                token = tokenBuilder->BuildLogToken(buf[tokenPos]);
                token->Initialize(formatter, "");
                AppendToken(token);

                formatter.Reset();
                state = TokenParseState::NormalState;

                buf.erase(tokenBeginPos);
            }
            else
            {
                buf.append(1, ch);
            }

            break;
        }
    }

    if (!buf.empty())
    {
        formatter.Reset();
        token = tokenBuilder->BuildLogToken(LLBC_LogTokenType::StrToken);
        token->Initialize(formatter, buf);
        AppendToken(token);

        buf.clear();
    }

    return LLBC_OK;
}

void LLBC_LogTokenChain::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    LLBC_BaseLogToken *token = _head;
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
        LLBC_BaseLogToken *next = _head->GetTokenNext();

        delete _head;
        _head = next;
    }
}

void LLBC_LogTokenChain::AppendToken(LLBC_BaseLogToken *token)
{
    token->SetTokenNext(nullptr);

    if (!_head)
    {
        _head = token;
        return;
    }

    LLBC_BaseLogToken *tmp = _head;
    while (tmp->GetTokenNext())
        tmp = tmp->GetTokenNext();

    tmp->SetTokenNext(token);
}

__LLBC_NS_END
