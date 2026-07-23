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

#include "core/Example_Core_Object.h"

namespace
{

// Deriving from LLBC_Object gives ref-counting (retain/release) and auto-release-pool capabilities
class MyObj final : public LLBC_Object
{
public:
    MyObj()
    {
        LLBC_PrintLn("  [MyObj construct]");
    }

    ~MyObj() override
    {
        LLBC_PrintLn("  [MyObj destruct]");
    }

    LLBC_String ToString() const override
    {
        return "MyObj instance";
    }
};

}

int Example_Core_Object::Run(int argc, char *argv[])
{
    // LLBC_Object provides ref-count-based memory management: a newly created object starts with ref count 1,
    // Retain() adds 1, Release() subtracts 1, and it is auto-deleted when the count reaches 0.

    // ========================= 1. Retain / Release =========================
    LLBC_PrintLn("Retain / Release ref-count:");
    MyObj *obj = new MyObj;                               // ref count = 1
    LLBC_PrintLn("- after new, refCount = %d", obj->GetRefCount());

    obj->Retain();                                        // ref count = 2
    LLBC_PrintLn("- after Retain, refCount = %d", obj->GetRefCount());
    LLBC_PrintLn("- ToString: %s", obj->ToString().c_str());

    obj->Release();                                       // ref count = 1
    LLBC_PrintLn("- after 1st Release, refCount = %d", obj->GetRefCount());
    obj->Release();                                       // ref count = 0, object destroyed
    LLBC_PrintLn("- after 2nd Release, obj destroyed");

    // ========================= 2. AutoReleasePool =========================
    LLBC_PrintLn("\nAutoReleasePool:");
    // Construct an auto-release pool (pushed onto the current thread's release-pool stack); on destruction it Releases all its objects.
    {
        LLBC_AutoReleasePool pool;

        MyObj *autoObj = new MyObj;                       // ref count = 1
        autoObj->AutoRelease();                           // hand over to the current release pool, no manual Release needed
        LLBC_PrintLn("- autoObj registered to pool, refCount = %d", autoObj->GetRefCount());
        LLBC_PrintLn("- leaving scope, pool will release it...");
    }   // pool destructed -> autoObj Released -> ref count reaches 0 -> destroyed
    LLBC_PrintLn("- pool destroyed, autoObj released");

    return LLBC_OK;
}
