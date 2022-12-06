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


#include "core/objbase/TestCase_ObjBase_Array.h"

namespace
{

class TestObj : public LLBC_Object
{
public:
    TestObj()
    {
        m_serialNum = ++ sm_serialNum;
        std::cout <<"TestObj construct: " <<this <<std::endl;
    }

    virtual ~TestObj()
    {
        std::cout <<"TestObj destruct: " <<this <<std::endl;
    }

public:
    virtual LLBC_String ToString() const
    {
        LLBC_String str;
        return str.append("Hello, I'm TestObj: ")
                  .append_format("%d", m_serialNum);
    }

    int GetSerialNum() const
    {
        return m_serialNum;
    }

private:
    int m_serialNum;
    static int sm_serialNum;
};

int TestObj::sm_serialNum = 0;

struct ForeachFun
{
    bool operator()(LLBC_Object *obj)
    {
        std::cout <<obj->ToString() <<std::endl;
        return true;
    }
};

struct ConstForeachFun
{
    bool operator()(const LLBC_Object *obj) const
    {
        std::cout <<obj->ToString() <<std::endl;
        return true;
    }
};

struct LessSortFun
{
    bool operator()(LLBC_Object *obj1, LLBC_Object *obj2)
    {
        TestObj *testObj1 = static_cast<TestObj *>(obj1);
        TestObj *testObj2 = static_cast<TestObj *>(obj2);

        return testObj1->GetSerialNum() < testObj2->GetSerialNum();
    }
};

struct GreaterSortFun
{
   bool operator()(LLBC_Object *obj1, LLBC_Object *obj2)
   {
       TestObj *testObj1 = static_cast<TestObj *>(obj1);
       TestObj *testObj2 = static_cast<TestObj *>(obj2);

       return testObj1->GetSerialNum() > testObj2->GetSerialNum();
   }
};

}

TestCase_ObjBase_Array::TestCase_ObjBase_Array()
{
}

TestCase_ObjBase_Array::~TestCase_ObjBase_Array()
{
}

int TestCase_ObjBase_Array::Run(int argc, char *argv[])
{
    std::cout <<"objbase/array test: " <<std::endl;

    BasicTest();
    IterTest();
    SortTest();

    std::cout <<"press any key to continue ..." <<std::endl;
    getchar();

    return 0;
}

void TestCase_ObjBase_Array::BasicTest()
{
    std::cout <<"Basic test: " <<std::endl;

    std::cout <<"Insert 10 objects." <<std::endl;
    LLBC_Array *arr = new LLBC_Array;
    const LLBC_Array &constArr = *arr;
    for(int i = 0; i < 10; ++i)
    {
        LLBC_Object *obj = new TestObj;
        arr->Insert(arr->End(), obj);
        obj->Release();
    }

    ForeachFun foreachFun;
    ConstForeachFun constForeachFun;
    std::cout <<"Done, output it: " <<std::endl;
    arr->Foreach(foreachFun);
    std::cout <<"Output it(using const foreach version method): " <<std::endl;
    constArr.Foreach(constForeachFun);

    std::cout <<"Using subscript to access array element." <<std::endl;
    for(int i = 0; i < 10; ++i)
    {
        std::cout <<(*arr)[i]->ToString() <<std::endl;
    }
    std::cout <<"Done" <<std::endl;

    std::cout <<"Using subscript to access array element(const array)." <<std::endl;
    for(int i = 0; i < 10; ++i)
    {
        std::cout <<constArr[i]->ToString() <<std::endl;
    }
    std::cout <<"Done" <<std::endl;

    std::cout <<"Fetch sub array in range[3, 5): " <<std::endl;
    LLBC_Array::IndexSet idxs;
    idxs.insert(3);
    idxs.insert(4);
    LLBC_Array *subArr = arr->ObjectsAtIndexs(idxs);
    std::cout <<"Done, output it: " <<std::endl;
    subArr->Foreach(foreachFun);

    delete arr;
    delete subArr;

    std::cout <<"Basic test done!" <<std::endl;
}

void TestCase_ObjBase_Array::IterTest()
{
    std::cout <<"Iter test: " <<std::endl;

    // PushBack/PushFront test.
    LLBC_Array arr;
    static const int insertCnt = 10;
    std::cout <<"PushBack/PushFront" <<insertCnt <<" objects into array." <<std::endl;
    for(int i = 1; i <= insertCnt; ++i)
    {
        LLBC_Object *obj = new TestObj;
        arr.PushBack(obj);

        obj->Release();
    }
    std::cout <<"Done, array size: " <<arr.GetSize() <<", Capacity: " <<arr.GetCapacity() <<std::endl;

    // Iterator test.
    ForeachFun foreachFun;
    std::cout <<"Iteration, using LLBC_Array::Iter" <<std::endl;
    LLBC_Array::Iter it1 = arr.Begin();
    for(; it1 != arr.End(); ++it1)
    {
        LLBC_Object *obj = *it1;
        std::cout <<obj->ToString() <<std::endl;
    }
    std::cout <<"Done" <<std::endl;

    std::cout <<"Iteration, using LLBC_Array::ReverseIter" <<std::endl;
    LLBC_Array::ReverseIter it2 = arr.ReverseBegin();
    for(; it2 != arr.ReverseEnd(); ++it2)
    {
        std::cout <<(*it2)->ToString() <<std::endl;
    }
    std::cout <<"Done" <<std::endl;

    std::cout <<"Insert new object to front" <<std::endl;
    LLBC_Object *obj = new TestObj;
    arr.Insert(arr.Begin(), obj);
    obj->Release();
    std::cout <<"Done, ouput it: " <<std::endl;
    arr.Foreach(foreachFun);

    std::cout <<"Insert new object before position 1: " <<std::endl;
    obj = new TestObj;
    arr.Insert(++ arr.Begin(), obj);
    obj->Release();
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);

    std::cout <<"Erase position 1's object: " <<std::endl;
    arr.Erase(arr.Begin() + 1);
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);

    std::cout <<"Erase position 0's object: " <<std::endl;
    arr.Erase(0);
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);

    std::cout <<"Erase range[begin, end): " <<std::endl;
    arr.Erase(arr.Begin(), arr.End());
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);

    std::cout <<"Iter test done" <<std::endl;
}

void TestCase_ObjBase_Array::SortTest()
{
    std::cout <<"Sort test: " <<std::endl;

    LLBC_Array arr;

    std::cout <<"Insert 10 elements: " <<std::endl;
    for(int i = 1; i <= 10; ++i)
    {
        LLBC_Object *obj = new TestObj;
        if(i % 2 == 0)
        {
            arr.PushBack(obj);
        }
        else
        {
            arr.PushFront(obj);
        }

        obj->Release();
    }

    ForeachFun foreachFun;
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);

    LessSortFun lessSortFun;
    std::cout <<"Sort it(use less than fun object): " <<std::endl;
    arr.Sort(lessSortFun);
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);

    GreaterSortFun greaterSortFun;
    std::cout <<"Sort it(use greater than fun object): " <<std::endl;
    arr.Sort(greaterSortFun);
    std::cout <<"Done, output it: " <<std::endl;
    arr.Foreach(foreachFun);
}
