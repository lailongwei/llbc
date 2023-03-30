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


#include "core/helper/TestCase_Core_Helper_StlHelper.h"

int TestCase_Core_Helper_StlHelper::Run(int argc, char *argv[])
{
    LLBC_PrintLn("LLBC_StlHelper test");

    // Vector test.
    std::vector<sint8 *> vec;
    for(sint32 i = 0; i < 10; ++i)
        vec.push_back(LLBC_Malloc(sint8, sizeof(sint8)));
    LLBC_STLHelper::FreeContainer(vec);
    LLBC_PrintLn("after free vector, size: %lu", vec.size());

    for(sint32 i = 0; i < 10; ++i)
        vec.push_back(new sint8);
    LLBC_STLHelper::DeleteContainer(vec, true, true);
    LLBC_PrintLn("after free vector, size: %lu", vec.size());

    // List test.
    std::list<sint32 *> list;
    for(sint32 i = 0; i < 10; ++i)
        list.push_back(LLBC_Malloc(sint32, sizeof(sint32)));
    LLBC_STLHelper::FreeContainer(list);
    LLBC_PrintLn("after free list, size: %lu", list.size());

    for(sint32 i = 0; i < 10; ++i)
        list.push_back(new sint32);
    LLBC_STLHelper::DeleteContainer(list, true, true);
    LLBC_PrintLn("after delete list, size: %lu", list.size());

    // Deque test.
    std::deque<sint32 *> dq;
    for(sint32 i = 0; i < 10; ++i)
        dq.push_back(LLBC_Malloc(sint32, sizeof(sint32)));
    LLBC_STLHelper::FreeContainer(dq);
    LLBC_PrintLn("after free deque, size: %lu", list.size());

    for(sint32 i = 0; i < 10; ++i)
        dq.push_back(new sint32);
    LLBC_STLHelper::DeleteContainer(dq, true, true);
    LLBC_PrintLn("after delete deque, size: %lu", list.size());

    // Stack test.
    std::stack<sint32 *> stk;
    for (sint32 i = 0; i < 10; ++i)
        stk.push(LLBC_Malloc(sint32, sizeof(sint32)));
    LLBC_STLHelper::FreeContainer(stk);
    LLBC_PrintLn("after free stack, size: %lu", stk.size());

    for (sint32 i = 0; i < 10; ++i)
        stk.push(new sint32);
    LLBC_STLHelper::DeleteContainer(stk);
    LLBC_PrintLn("after delete stack, size: %lu", stk.size());

    // Queue test.
    std::queue<sint32 *> q;
    for (sint32 i = 0; i < 10; ++i)
        q.push(LLBC_Malloc(sint32, sizeof(sint32)));
    LLBC_STLHelper::FreeContainer(q);
    LLBC_PrintLn("after free queue, size: %lu", q.size());

    for (sint32 i = 0; i < 10; ++i)
        q.push(new sint32);
    LLBC_STLHelper::DeleteContainer(q);
    LLBC_PrintLn("after delete queue, size: %lu", q.size());

    // Map test.
    std::map<sint32, sint32 *> map1;
    for(sint32 i = 0; i < 10; ++i)
        map1[i] = LLBC_Malloc(sint32, sizeof(sint32));
    LLBC_STLHelper::FreeContainer(map1);
    LLBC_PrintLn("after free map, size: %lu", map1.size());

    std::map<sint32, std::string *> map2;
    for(sint32 i = 0; i < 10; ++i)
        map2[i] = new std::string;
    LLBC_STLHelper::DeleteContainer(map2, true, true);
    LLBC_PrintLn("after delete map, size: %lu", map2.size());

    // Set test.
    std::set<sint32 *> s;
    for(sint32 i = 0; i < 10; ++i)
        s.insert(LLBC_Malloc(sint32, sizeof(sint32)));
    LLBC_STLHelper::FreeContainer(s);
    LLBC_PrintLn("after free set, size: %lu", s.size());

    for(sint32 i = 0;  i < 10; ++i)
        s.insert(new sint32);
    LLBC_STLHelper::DeleteContainer(s, true, true);
    LLBC_PrintLn("after delete set, size: %lu", s.size());

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
