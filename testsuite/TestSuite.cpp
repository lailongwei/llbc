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


#include "TestSuite.h"
#include "TestSuiteDef.h"

#define PrintLineC(color, fmt, ...)                              \
	old = LLBC_GetConsoleColor(stdout);                          \
	LLBC_SetConsoleColor(stdout, color);                         \
	LLBC_NS __LLBC_FilePrint(false, stdout, fmt, ##__VA_ARGS__); \
	LLBC_SetConsoleColor(stdout, old);                           \
	LLBC_NS __LLBC_FilePrint(true, stdout, "");\

#define DEPARATION_CHARACTER "--------------------------------------------------"

int TestSuite_Main(int argc, char *argv[])
{
    ::llbc::LLBC_Startup();
	TraitsLoop<test_case_count>::generate();

	int old;
	while (true)
	{
		PrintLineC(LLBC_NS LLBC_ConsoleColor::Bg_Green, DEPARATION_CHARACTER);
		for (int i = 0; i < test_case_count; ++i)
		{
			auto name = TEST_CASE_NAME(i);
			auto func = TEST_CASE_FUNC(i);
			if (name == nullptr || func == nullptr)
				continue;

			LLBC_PrintLine("%d: %s", i, name);
		}
		PrintLineC(LLBC_NS LLBC_ConsoleColor::Bg_Green, DEPARATION_CHARACTER);

		LLBC_Print("Please input:\t");
		int idx = -1;
		if (fscanf(stdin, "%d", &idx) != 1)
			continue;

		std::getchar();  //清除缓冲区回车字符
		if (idx < 0 || idx >= test_case_count)
			break;

		auto name = TEST_CASE_NAME(idx);
		auto func = TEST_CASE_FUNC(idx);
		if (name == nullptr || func == nullptr)
		{
			LLBC_PrintLine("unimplemented test case.");
			continue;
		}

		::llbc::LLBC_ITestCase* test = func();
		if (!test)
		{
			LLBC_PrintLine("unimplemented test case.");
			continue;
		}

		PrintLineC(LLBC_NS LLBC_ConsoleColor::Bg_White, "%s selected.", name);

		test->Run(argc, argv);
		LLBC_Delete(test);
	}

    ::llbc::LLBC_Cleanup();
    return 0;
}
