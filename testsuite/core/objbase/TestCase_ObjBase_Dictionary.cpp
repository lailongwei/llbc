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


#include "core/objbase/TestCase_ObjBase_Dictionary.h"

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
        std::cout <<"Testobj destruct: " <<this <<std::endl;
    }

public:
    virtual LLBC_String ToString() const
    {
        LLBC_String str;
        str.append_format("TestObj, serial num: %d", m_serialNum);

        return str;
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
    void operator()(LLBC_DictionaryElem *elem)
    {
        LLBC_String str;
        str.append_format("key(type:%s): ", (elem->IsIntKey() ? "Int" : "String"));
        if(elem->IsIntKey())
        {
            str.append_format("%d", elem->GetIntKey());
        }
        else
        {
            str.append_format("%s", elem->GetStrKey().c_str());
        }

        str.append_format(", value: %s", elem->GetObject()->ToString().c_str());

        std::cout <<str <<std::endl;
    }
};

struct LessThanFun
{
    bool operator()(const LLBC_DictionaryElem *elem1, const LLBC_DictionaryElem *elem2) const
    {
        const TestObj *obj1 = static_cast<const TestObj *>(elem1->GetObject());
        const TestObj *obj2 = static_cast<const TestObj *>(elem2->GetObject());

        return obj1->GetSerialNum() < obj2->GetSerialNum();
    }
};

struct GreaterThanFun
{
    bool operator()(const LLBC_DictionaryElem *elem1, const LLBC_DictionaryElem *elem2) const
    {
        const TestObj *obj1 = static_cast<const TestObj *>(elem1->GetObject());
        const TestObj *obj2 = static_cast<const TestObj *>(elem2->GetObject());

        return obj1->GetSerialNum() > obj2->GetSerialNum();
    }
};

}

TestCase_ObjBase_Dictionary::TestCase_ObjBase_Dictionary()
{
}

TestCase_ObjBase_Dictionary::~TestCase_ObjBase_Dictionary()
{
}

int TestCase_ObjBase_Dictionary::Run(int argc, char *argv[])
{
    std::cout <<"objbase/dictionary test: " <<std::endl;

    LLBC_Dictionary dict;

    // Insert 10 int type object.
    std::cout <<"Inset 10 integer type objects." <<std::endl;
    for(int i = 1; i <= 10; ++i)
    {
        LLBC_Object *obj = new TestObj;
        dict.Insert(i, obj);

        obj->Release();
    }

    ForeachFun foreachFun;
    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    // Rehash, set bucket size.
    const LLBC_Dictionary::size_type bucketSize = 7;
    std::cout <<"Set bucket size to " <<bucketSize <<std::endl;
    dict.SetHashBucketSize(bucketSize);
    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    // Insert string key type object.
    std::cout <<"Insert string type key to dictionary" <<std::endl;
    LLBC_Object *obj = new TestObj;
    dict.Insert("Hello, World!!!", obj);
    obj->Release();
    
    obj = new TestObj;
    dict.Insert("sunday", obj);
    obj->Release();

    obj = new TestObj;
    dict.Insert("saturday", obj);
    obj->Release();

    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    // Insert integer type key again.
    std::cout <<"Insert integer type key again, 10 elements." <<std::endl;
    for(int i = 11; i <= 20; ++i)
    {
        LLBC_Object *obj = new TestObj;
        dict.Insert(i, obj);

        obj->Release();
    }

    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    // Sort test.
    GreaterThanFun greaterThanFun;
    std::cout <<"Sort it, using greater than function object." <<std::endl;
    dict.Sort(greaterThanFun);
    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    LessThanFun lessThanFun;
    std::cout <<"Sort it, using less than function object." <<std::endl;
    dict.Sort(lessThanFun);
    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    // Erase value.
    std::cout <<"Erase all integer type values." <<std::endl;
    for(int i = 1; i < 20; i += 2)
    {
        LLBC_Dictionary::Iter it = dict.Find(i);
        ASSERT(it != dict.End() && "Dictionary internal error, check it!");
        dict.Erase(it);
    }
    for(int i = 2; i <= 20; i += 2)
    {
        int status = dict.Erase(i);
        ASSERT(status == LLBC_OK && "Dictionary internal error, check it!");
    }

    std::cout <<"Done, output: " <<std::endl;
    dict.Foreach(foreachFun);

    // Use reverse iter to dump data.
    std::cout <<"Using ReverseIter to dump data: " <<std::endl;
    LLBC_Dictionary::ConstReverseIter reverseIt = dict.ReverseBegin();
    for(; reverseIt != dict.ReverseEnd(); ++reverseIt)
    {
        std::cout <<(*reverseIt)->ToString() <<std::endl;
    }
    std::cout <<std::endl;

    std::cout <<"Press any key to continue ..." <<std::endl;
    getchar();

    return 0;
}
