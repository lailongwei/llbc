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

#include "core/Example_Core_ObjPool.h"

namespace
{

// Pooled object: deriving from LLBC_PoolObj makes it poolable; Reuse() is called when the object is reused, to reset its state.
class Bullet final : public LLBC_PoolObj
{
public:
    Bullet() : _id(0)
    {
        LLBC_PrintLn("  [Bullet construct]");
    }

    ~Bullet() override
    {
        LLBC_PrintLn("  [Bullet destruct]");
    }

    // Reuse callback: called when the object is fetched from the pool again
    void Reuse()
    {
        LLBC_PrintLn("  [Bullet reuse]");
        _id = 0;
    }

    void SetId(int id) { _id = id; }
    int GetId() const { return _id; }

private:
    int _id;
};

}

int Example_Core_ObjPool::Run(int argc, char *argv[])
{
    // New object pool LLBC_ObjPool: caches objects by type; Acquire<T>() takes one out, Release() returns it for reuse,
    // reducing the overhead of frequent new/delete. (Note: only the new object pool is demonstrated, no legacy object pool is involved)

    LLBC_ObjPool objPool;

    // ========================= 1. Acquire / Release =========================
    LLBC_PrintLn("Acquire / Release:");
    // The first Acquire actually constructs a Bullet
    Bullet *b1 = objPool.Acquire<Bullet>();
    b1->SetId(1001);
    LLBC_PrintLn("- acquired b1(%p), id = %d", b1, b1->GetId());

    // Return it to the pool (the object is not destructed here, just recycled for reuse)
    objPool.Release(b1);
    LLBC_PrintLn("- released b1 back to pool");

    // Acquire again, reusing the just-returned object (triggers Reuse(), usually the same address)
    Bullet *b2 = objPool.Acquire<Bullet>();
    LLBC_PrintLn("- acquired b2(%p) reused, id = %d", b2, b2->GetId());
    objPool.Release(b2);

    // ========================= 2. Guarded object (RAII auto-release) =========================
    LLBC_PrintLn("\nGuarded pooled object(auto release on scope exit):");
    {
        // AcquireGuarded returns a guard object that auto-returns to the pool on scope exit, no manual Release needed
        LLBC_GuardedPoolObj<Bullet> guarded = objPool.AcquireGuarded<Bullet>();
        guarded->SetId(2002);                             // operator-> directly accesses the guarded object
        LLBC_PrintLn("- guarded valid: %s, id = %d", guarded ? "true" : "false", guarded->GetId());
        LLBC_PrintLn("- leaving scope, guarded will auto-release...");
    }   // guarded destructed -> auto-returned to the pool
    LLBC_PrintLn("- guarded auto-released");

    return LLBC_OK;
}
