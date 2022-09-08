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

#include "comm/TestCase_Comm_CompEvent.h"

int TestCase_Comm_CompEvent::Run(int argc, char *argv[])
{
    std::cout << "ComponentEvent test:" << std::endl;

    std::cout << "Component events list:" << std::endl;
    for (int i = LLBC_ComponentEventIndex::Begin; i < LLBC_ComponentEventIndex::End; ++i)
        fprintf(stdout, "- Comp event:%08llu index:%d\n", 1llu << i, LLBC_ComponentEvents::IndexOf(1llu << i));

    fprintf(stdout, "All events:%08llu, Default events:%08llu\n",
            LLBC_ComponentEvents::AllEvents, LLBC_ComponentEvents::DefaultEvents);

    std::cout << "Press any key to continue..." << std::endl;
    getchar();

    return LLBC_OK;
}
