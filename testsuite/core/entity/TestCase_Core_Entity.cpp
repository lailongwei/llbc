/**
 * @file    TestCase_Core_Entity.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/13
 * @version 1.0
 *
 * @brief
 */

#include "core/entity/TestCase_Core_Entity.h"

TestCase_Core_Entity::TestCase_Core_Entity()
{
}

TestCase_Core_Entity::~TestCase_Core_Entity()
{
}

int TestCase_Core_Entity::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/entity test:");

    LLBC_Entity entity;
    entity.AddProperty("sint8Prop", LLBC_Variant((sint8)-8));
    entity.AddProperty("uint8Prop", LLBC_Variant((uint8)8));
    entity.AddProperty("sint16Prop", LLBC_Variant((sint16)-16));
    entity.AddProperty("uint16Prop", LLBC_Variant((uint16)16));
    entity.AddProperty("sint32Prop", LLBC_Variant((sint32)-32));
    entity.AddProperty("uint32Prop", LLBC_Variant((uint32)32));
    entity.AddProperty("sint64Prop", LLBC_Variant((sint64)-64));
    entity.AddProperty("uint64Prop", LLBC_Variant((uint64)64));

    entity.AddBehavior(1, new int, "behavior1");
    entity.AddBehavior(2, new uint64, "behavior2");
    entity.AddBehavior(3, new LLBC_String, "behavior3");

    std::cout <<entity.GetProperty("sint8Prop") <<std::endl;
    std::cout <<entity.GetProperty("uint8Prop") <<std::endl;
    std::cout <<entity.GetProperty("sint16Prop") <<std::endl;
    std::cout <<entity.GetProperty("uint16Prop") <<std::endl;
    std::cout <<entity.GetProperty("sint32Prop") <<std::endl;
    std::cout <<entity.GetProperty("uint32Prop") <<std::endl;
    std::cout <<entity.GetProperty("sint64Prop") <<std::endl;
    std::cout <<entity.GetProperty("uint64Prop") <<std::endl;
    std::cout <<entity.GetProperty("ptrProp") <<std::endl;

    int *behavior1 = entity.GetBehavior<int>(1);
    std::cout <<"behavior1 " <<behavior1 <<std::endl;
    uint64 *behavior2 = entity.GetBehavior<uint64>("behavior2");
    std::cout <<"behavior2 " <<behavior2 <<std::endl;
    std::cout <<"behavior3 " <<entity.GetBehavior<LLBC_String>(3) <<std::endl;

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
