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
