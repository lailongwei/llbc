/**
 * @file    TestCase_ObjBase_Object.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/30
 * @version 1.0
 *
 * @brief
 */

#include "objbase/TestCase_ObjBase_Object.h"

#if LLBC_CFG_OBJBASE_ENABLED

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

#endif // LLBC_CFG_OBJBASE_ENABLED
