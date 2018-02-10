-- Monkey patch current lua engine.
-- 1. patch table.
--   1.1 patch table.concat, after patched, table.concat() can concat any implemented __tostring's type objects.
--   1.1 patch table.readonly, after patched, table.readonly() can make any table to become readonly.
--
-- 2. patch _G, add some util/helper functions.
--   2.1 add hash method.
--   2.2 add printf method.
--   2.3 add topointer method.
--   2.4 add GUID support methods.
--   2.5 add FuncTool support methods.
--
-- 3. Patch _G, add some object oriented about methods.
--   3.1. patch newclass() method to _G, use to create class.
--   3.2. patch getclass() method to _G, use to get class definition by name.
--   3.3. patch isinstance() method to _G, use to determine object type(included llbc style class type and normal lua type).
--
-- 5. patch _G, add log about support classes.
--
-- 4. patch _G, add timer about support classes.
--
-- :returns: no return.
function llbc.monkey_patch()
    -- patch table.
    table.concat = _llbc.Util_Table_Concat
    table.readonly = llbc.set_table_readonly

    -- patch some util functions to _G.
    hash = llbc.hash
    printf = llbc.printf
    topointer = _llbc.Util_Misc_ToPointer
    GUID = llbc.GUID
    FuncTool = llbc.FuncTool

    -- patch obiect oriented functions to _G.
    newclass = llbc.newclass
    getclass = llbc.getclass
    isinstance = llbc.isinstance

    -- patch log about classes to _G.
    Log = llbc.Log

    -- patch timer support classes to _G.
    Timer = llbc.Timer
    TimerScheduler = llbc.TimerScheduler
end
