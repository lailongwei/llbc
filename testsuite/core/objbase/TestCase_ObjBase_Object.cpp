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


#include "core/objbase/TestCase_ObjBase_Object.h"

namespace
{

class MyObj : public LLBC_Object
{
public:
    MyObj()
    {
        std::cout <<"MyObj construct" <<std::endl;
        m_data = malloc(10);
    }

    virtual ~MyObj()
    {
        std::cout <<"MyObj destruct" <<std::endl;
        free(m_data);
    }

private:
    void *m_data;
};

}

TestCase_ObjBase_Object::TestCase_ObjBase_Object()
{
}

TestCase_ObjBase_Object::~TestCase_ObjBase_Object()
{
}

int TestCase_ObjBase_Object::Run(int arg, char *argv[])
{
    std::cout <<"objbase/object test: " <<std::endl;
    
    MyObj *obj = new MyObj;
    obj->Retain();
    obj->Release();
    obj->Release();

    MyObj anotherObj;

    std::cout <<"press any key to continue ..." <<std::endl;
    getchar();

    return 0;
}
