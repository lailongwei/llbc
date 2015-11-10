/**
 * @file    TestCase_Core_Helper_JVHelper.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/05/05
 * @version 1.0
 *
 * @brief
 */

#include "core/helper/TestCase_Core_Helper_JVHelper.h"

TestCase_Core_Helper_JVHelper::TestCase_Core_Helper_JVHelper()
{
}

TestCase_Core_Helper_JVHelper::~TestCase_Core_Helper_JVHelper()
{
}

int TestCase_Core_Helper_JVHelper::Run(int argc, char *argv[])
{
    std::cout <<"core/helper/JVHelper test:" <<std::endl;

    LLBC_String jsonStr = "{"
                            "    \"intVal\" : 3,"
                            "    \"doubleVal\" : 0.618,"
                            "    \"strVal\" : \"Hello World\","
                            "    \"arrVal\": [1, 2.0, \"Hey, Judy\"],"
                            "    \"dictVal\" : { \"embedded\" : \"The embed value!\" }"
                            "}";

    LLBC_JsonValue json;
    Json::Reader reader;
    if( !reader.parse(jsonStr, json) )
    {
        std::cout <<"Parse json data failed!" <<std::endl;

        std::cout <<"Json data: " <<std::endl;
        std::cout <<jsonStr <<std::endl;

        return LLBC_ERROR_UNKNOWN;
    }

    std::cout <<"Test JSON->VARIANT: " <<std::endl;
    LLBC_Variant var = LLBC_JVHelper::J2V(json);
    std::cout <<"Json data: " <<std::endl;
    std::cout <<jsonStr <<std::endl;
    std::cout <<"Variant data: " <<std::endl;
    std::cout <<var <<std::endl;
    std::cout <<std::endl;

    std::cout <<"Test VARIANT->JSON: " <<std::endl;
    json = LLBC_JVHelper::V2J(var);
    std::cout <<"Variant data: " <<std::endl;
    std::cout <<var <<std::endl;
    std::cout <<"Json data: " <<std::endl;
    std::cout <<json.toStyledString() <<std::endl;
    std::cout <<std::endl;

    std::cout <<"Press any key to continue ...";
    getchar();

    return LLBC_ERROR_SUCCESS;
}
