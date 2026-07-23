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

#include "core/Example_Core_Delegate.h"

namespace
{

// Free function for the binding demo
int Add(int a, int b)
{
    return a + b;
}

// Class for the binding demo, with a normal member function and a const member function
class Calculator
{
public:
    explicit Calculator(int base) : _base(base) {}

    // Normal member function
    int AddBase(int v)
    {
        return _base + v;
    }

    // const member function
    int MulBase(int v) const
    {
        return _base * v;
    }

private:
    int _base;
};

}

int Example_Core_Delegate::Run(int argc, char *argv[])
{
    // LLBC_Delegate<Rtn(Args...)> is a unified callable wrapper that can bind
    // a free function / lambda / member function, and is invoked via operator().

    // ========================= 1. Bind a free function =========================
    LLBC_PrintLn("Bind a free function:");
    LLBC_Delegate<int(int, int)> freeDeleg(&Add);
    LLBC_PrintLn("- freeDeleg(3, 4) = %d", freeDeleg(3, 4));

    // ========================= 2. Bind a lambda / callable object =========================
    LLBC_PrintLn("\nBind a lambda:");
    int factor = 10;
    LLBC_Delegate<int(int, int)> lambdaDeleg = [factor](int a, int b) -> int {
        return (a + b) * factor;
    };
    LLBC_PrintLn("- lambdaDeleg(3, 4) = %d", lambdaDeleg(3, 4));

    // ========================= 3. Bind member functions =========================
    LLBC_PrintLn("\nBind member functions:");
    Calculator calc(100);
    // Bind a normal member function: (object pointer, &Class::method)
    LLBC_Delegate<int(int)> methDeleg(&calc, &Calculator::AddBase);
    LLBC_PrintLn("- methDeleg(5) = %d", methDeleg(5));

    // Bind a const member function
    LLBC_Delegate<int(int)> constMethDeleg(&calc, &Calculator::MulBase);
    LLBC_PrintLn("- constMethDeleg(5) = %d", constMethDeleg(5));

    // ========================= 4. Validity check =========================
    LLBC_PrintLn("\nValidate delegate:");
    LLBC_Delegate<int(int)> emptyDeleg;
    LLBC_PrintLn("- emptyDeleg valid: %s", emptyDeleg ? "true" : "false");
    LLBC_PrintLn("- methDeleg valid : %s", methDeleg ? "true" : "false");

    return LLBC_OK;
}
