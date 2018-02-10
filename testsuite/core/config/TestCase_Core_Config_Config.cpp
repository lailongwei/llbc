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


#include "core/config/TestCase_Core_Config_Config.h"

TestCase_Core_Config_Config::TestCase_Core_Config_Config()
{
}

TestCase_Core_Config_Config::~TestCase_Core_Config_Config()
{
}

int TestCase_Core_Config_Config::Run(int argc, char *argv[])
{
    std::cout <<"LLBC_Config test: " <<std::endl;

#if LLBC_TARGET_PLATFORM_NON_IPHONE
    const char *file = "core/config/test_json.json";
#else
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    const char *file = mainBundle->GetResPath("test_json", "json").c_str();
    std::cout <<"json file path: " <<file <<std::endl;
#endif

    LLBC_Config config;
    if(config.AddFile(file) != LLBC_OK)
    {
        std::cerr <<"Add file to config failed, file: " <<file;
        std::cerr <<", error desc: " <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }

    if(config.Initialize() != LLBC_OK)
    {
        std::cerr <<"Initialize config failed, error desc: ";
        std::cerr <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }

    LLBC_JsonValue jsonValue = config.GetJsonValue(file, "json_value");
    std::cout <<"json_value: " <<std::endl;
    std::cout <<jsonValue.toStyledString() <<std::endl;

    LLBC_Variant intVal = config.GetVariantValue(file, "int_value.int_value");
    std::cout <<"int_value.int_value: " <<intVal <<std::endl;
    LLBC_Variant stringVal = config.GetVariantValue(file, "string_value");
    std::cout <<"string_value: " <<stringVal <<std::endl;

    std::vector<LLBC_JsonValue> jsonArray = config.GetJsonValueArray(file, "json_array");
    std::cout <<"json_array[count: " <<jsonArray.size() <<"]: " <<std::endl;
    for(size_t i = 0; i < jsonArray.size(); i ++)
    {
        std::cout <<jsonArray[i].toStyledString() <<std::endl;
    }

    std::map<int, LLBC_JsonValue> intJsonMap = config.GetIntJsonMap(file, "int_json_map");
    std::cout <<"int_json_map[count: " <<intJsonMap.size() <<"]: " <<std::endl;
    std::map<int, LLBC_JsonValue>::const_iterator intJsonMapIter = intJsonMap.begin();
    for(; intJsonMapIter != intJsonMap.end(); intJsonMapIter ++)
    {
        std::cout <<"key: " <<intJsonMapIter->first 
            <<"\nvalue: " <<intJsonMapIter->second.toStyledString() <<std::endl;
    }

    std::map<LLBC_String, LLBC_JsonValue> stringJsonMap = config.GetStringJsonMap(file, "string_json_map");
    std::cout <<"string_json_map[count: " <<stringJsonMap.size() <<"]: " <<std::endl;
    std::map<LLBC_String, LLBC_JsonValue>::const_iterator stringJsonMapIter = stringJsonMap.begin();
    for(; stringJsonMapIter != stringJsonMap.end(); stringJsonMapIter ++)
    {
        std::cout <<"key: " <<stringJsonMapIter->first
            <<"\nvalue: " <<stringJsonMapIter->second.toStyledString() <<std::endl;
    }

    std::vector<LLBC_Variant> variantArray = config.GetVariantValueArray(file, "variant_array");
    std::cout <<"variant_array[count: " <<variantArray.size() <<"]: " <<std::endl;
    for(size_t i = 0; i < variantArray.size(); i ++)
    {
        std::cout <<variantArray[i] <<std::endl;
    }

    std::map<int, LLBC_Variant> intVariantMap = config.GetIntVariantMap(file, "int_variant_map");
    std::cout <<"int_variant_map[count: " <<intVariantMap.size() <<"]: " <<std::endl;
    std::map<int, LLBC_Variant>::const_iterator intVariantMapIter = intVariantMap.begin();
    for(; intVariantMapIter != intVariantMap.end(); intVariantMapIter ++)
    {
        std::cout <<"key: " <<intVariantMapIter->first
            <<"\nvalue: " <<intVariantMapIter->second <<std::endl;
    }

    std::map<LLBC_String, LLBC_Variant> stringVariantMap = 
        config.GetStringVariantMap(file, "string_variant_map.string_variant_map");
    std::cout <<"string_variant_map.string_variant_map[count: " 
        <<stringVariantMap.size() <<"]: " <<std::endl;
    std::map<LLBC_String, LLBC_Variant>::const_iterator stringVariantMapIter = stringVariantMap.begin();
    for(; stringVariantMapIter != stringVariantMap.end(); stringVariantMapIter ++)
    {
        std::cout <<"key: " <<stringVariantMapIter->first
            <<"\nvalue: " <<stringVariantMapIter->second <<std::endl;
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}
