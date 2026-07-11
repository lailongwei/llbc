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

#include "core/utils/TestCase_Core_Utils_Misc.h"

int TestCase_Core_Utils_Misc::Run(int argc, char *argv[])
{
    std::cout <<"core/utils/Util_Misc test:" <<std::endl;
    TestStartArgs(argc, argv);

    std::cout <<"Press any key to continue...";
    getchar();

    return LLBC_OK;
}

int TestCase_Core_Utils_Misc::TestStartArgs(int argc, char *argv[])
{
    std::cout <<"test startargs:" <<std::endl;
    LLBC_StartArgs startArgs;
    int ret = startArgs.Parse(argc, argv);
    if (ret != LLBC_OK)
    {
        std::cerr << "Failed to parse startup arguments, error: " << LLBC_FormatLastError() << std::endl;
        return -1;
    }

    std::cout <<"Module file path: " <<startArgs.GetModuleFilePath() <<std::endl;
    std::cout <<"Arguments(total:" <<startArgs.GetArgumentsCount() <<"):" <<std::endl;
    for (size_t i = 0; i < startArgs.GetArgumentsCount(); ++i)
        std::cout <<"  " <<i <<startArgs[i] <<std::endl;

    std::cout <<"Naming startup arguments(total:" <<startArgs.GetNamingArgumentsCount() <<"):" <<std::endl;
    for (std::map<LLBC_String, LLBC_Variant>::const_iterator it = startArgs.GetAllNamingArguments().begin();
         it != startArgs.GetAllNamingArguments().end();
         it++)
        std::cout <<"  " <<it->first <<": " <<it->second <<std::endl;

    return 0;
}
