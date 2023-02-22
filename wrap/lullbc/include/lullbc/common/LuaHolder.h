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

#include "lullbc/common/Export.h"
#include "lullbc/common/LibHeader.h"

#pragma once

/**
 * \brief The lua holder class encapsulation.
 */
class LULLBC_HIDDEN lullbc_LuaHolder
{
public:
    /**
     * Hold lua object from stack, index by <toKey>.
     * @param[in] l            - the lua vm.
     * @param[in] fromStackPos - the lua object index on stack.
     * @param[in] toKey        - index key in holder.
     * @param[in] remove       - when the lua object holded, remove from stack or not, default not remove.
     */
    static void Hold(lua_State *l, int fromStackPos, const char *toKey, bool remove = false);
    /**
     * @param[in] l             - the lua vm.
     * @param[in] fromGlobalKey - the lua object key on _G table.
     * @param[in] toKey         - index key in holder.
     * @param[in] remove        - when the lua object holded, remove from _G or not, default not remove.
     */
    static void Hold(lua_State *l, const char *fromGlobalKey, const char *toKey, bool remove = false);

    /**
     * UnHold lua object from holder.
     * @param[in] l   - the lua vm.
     * @param[in] key - the lua object key in holder.
     */
    static void UnHold(lua_State *l, const char *key);

public:
    /**
     * Get lua object from lua holder.
     * @param[in] l   - the lua vm.
     * @param[in] key - the lua object key.
     */
    static void Get(lua_State *l, const char *key);

private:
    static void PushHoldTable(lua_State *l);

private:
    LLBC_DISABLE_ASSIGNMENT(lullbc_LuaHolder);
};
