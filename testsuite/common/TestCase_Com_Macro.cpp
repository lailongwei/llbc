#include "common/TestCase_Com_Macro.h"

int TestCase_Com_Macro::Run(int argc, char** argv)
{
    LLBC_PrintLn("Common/Marco test:");

    // LLBC_Foreach test.
    std::vector<int> vec(3, 1);
    LLBC_Foreach(vec, LLBC_PrintLn("Foreach test:%d", item));

    // LLBC_DoIf test.
    for(;;)
    {
        LLBC_PrintLn("DoIf test");
        LLBC_DoIf(true, break);
    }

    // LLBC_ContinueIf test.
    for(int i = 0; i < 10; ++i)
    {
        LLBC_ContinueIf(i % 2 == 0);
        LLBC_PrintLn("ContinueIf test: %d", i);
    }

    // LLBC_BreakIf test.
    for(int i = 0; i < 10; ++i)
    {
        LLBC_BreakIf(i == 5);
        LLBC_PrintLn("BreakIf test: %d", i);
    }

    // LLBC_ReturnIf test.
    auto ret = []() -> int
    {
        LLBC_PrintLn("ReturnIf test");
        LLBC_ReturnIf(true, 1);
        return 0;
    }();
    LLBC_PrintLn("ReturnIf test, ret: %d", ret);

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return LLBC_OK;
}
