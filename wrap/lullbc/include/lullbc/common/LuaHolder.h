/**
 * @file    LuaHolder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-16
 * @version
 *
 * @brief   The lua holder class encapsulation.
 */
#include "lullbc/common/Export.h"
#include "lullbc/common/LibHeader.h"

#ifndef __LULLBC_COM_LUA_HOLDER_H__
#define __LULLBC_COM_LUA_HOLDER_H__

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

#endif // !__LULLBC_COM_LUA_HOLDER_H__
