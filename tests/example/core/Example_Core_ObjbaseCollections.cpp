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

#include "core/Example_Core_ObjbaseCollections.h"

namespace
{

// Elements stored in LLBC_Array / LLBC_Dictionary must be LLBC_Object-derived classes
class Item final : public LLBC_Object
{
public:
    explicit Item(const LLBC_String &name = "") : _name(name) {}

    LLBC_String ToString() const override
    {
        return LLBC_String().format("Item[%s]", _name.c_str());
    }

private:
    LLBC_String _name;
};

}

int Example_Core_ObjbaseCollections::Run(int argc, char *argv[])
{
    // LLBC_Array / LLBC_Dictionary are ordered containers based on ref-counting (LLBC_Object);
    // the container Retains an element on insert and Releases it on removal/destruction.

    // ========================= 1. LLBC_Array =========================
    LLBC_PrintLn("LLBC_Array insert / iterate / subscript:");
    LLBC_Array arr;
    for (int i = 0; i < 3; ++i)
    {
        Item *item = new Item(LLBC_String().format("arr-%d", i));
        arr.PushBack(item);   // container holds it (Retain)
        item->Release();      // release the local reference, hand over to the container
    }
    LLBC_PrintLn("- array size = %ld", arr.GetSize());

    // Iterate via iterator
    LLBC_PrintLn("- iterate by Iter:");
    for (LLBC_Array::Iter it = arr.Begin(); it != arr.End(); ++it)
        LLBC_PrintLn("  - %s", (*it)->ToString().c_str());

    // Subscript access
    LLBC_PrintLn("- subscript arr[1]: %s", arr[1]->ToString().c_str());

    // ========================= 2. LLBC_Dictionary =========================
    LLBC_PrintLn("\nLLBC_Dictionary insert / find / iterate:");
    LLBC_Dictionary dict;

    // Integer key
    for (int i = 1; i <= 3; ++i)
    {
        Item *item = new Item(LLBC_String().format("int-%d", i));
        dict.Insert(i, item);
        item->Release();
    }
    // String key
    Item *namedItem = new Item("named-value");
    dict.Insert("myKey", namedItem);
    namedItem->Release();

    LLBC_PrintLn("- dict size = %ld", dict.GetSize());

    // Find by key
    LLBC_Dictionary::Iter found = dict.Find(2);
    if (found != dict.End())
        LLBC_PrintLn("- find int key 2: %s", found.Obj()->ToString().c_str());

    LLBC_Dictionary::Iter foundStr = dict.Find("myKey");
    if (foundStr != dict.End())
        LLBC_PrintLn("- find str key \"myKey\": %s", foundStr.Obj()->ToString().c_str());

    // Iterate (distinguishing integer key / string key)
    LLBC_PrintLn("- iterate all:");
    for (LLBC_Dictionary::Iter it = dict.Begin(); it != dict.End(); ++it)
    {
        if (it.IsIntKey())
            LLBC_PrintLn("  - key(int)=%d => %s", it.IntKey(), it.Obj()->ToString().c_str());
        else
            LLBC_PrintLn("  - key(str)=%s => %s", it.StrKey().c_str(), it.Obj()->ToString().c_str());
    }

    return LLBC_OK;
}
