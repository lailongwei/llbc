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


#include "core/config/TestCase_Core_Config_Properties.h"

TestCase_Core_Config_Properties::TestCase_Core_Config_Properties()
{
}

TestCase_Core_Config_Properties::~TestCase_Core_Config_Properties()
{
}

int TestCase_Core_Config_Properties::Run(int argc, char *argv[])
{
    std::cout <<"LLBC_Properties test: " <<std::endl;

	// Define properties dump lambda.
	auto dumpProperties = [](const LLBC_Variant &properties) {
        std::cout <<"  Fetch llbc: '" <<properties["llbc"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch llbc.test: '" <<properties["llbc"]["test"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch llbc.test.intVal: '" <<properties["llbc"]["test"]["intVal"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch llbc.test.boolFalse: '" <<properties["llbc"]["test"]["boolFalse"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch llbc.test.boolTrue: '" <<properties["llbc"]["test"]["boolTrue"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch llbc.test.strVal: '" <<properties["llbc"]["test"]["strVal"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch llbc.test.strVal2: '" <<properties["llbc"]["test"]["strVal2"].ValueToString() <<"'" <<std::endl;
        std::cout <<"  Fetch xxx: " <<properties["xxx"].ValueToString() <<std::endl;
    };

    // Construct property.
    LLBC_Variant property;
    std::cout <<"Basic test, Construct property:" <<std::endl;
    property["llbc"]["test"]["intVal"] = 30;
    property["llbc"]["test"]["boolFalse"] = false;
    property["llbc"]["test"]["boolTrue"] = true;
    property["llbc"]["test"]["strVal"] = "Hello #World!";
    property["llbc"]["test"]["strVal2"] = "  Hello #World!\\\\  #";

    // Test: SaveToString()
    LLBC_String errMsg;
    LLBC_String content;
    LLBC_ErrorAndReturnIfNot(LLBC_Properties::SaveToString(property, content, &errMsg) == LLBC_OK,
                             LLBC_FAILED,
                             "SaveToString failed, error:%s", errMsg.c_str());
    std::cout <<"After save to string, string content is:\n" <<content <<std::endl;

    // Test: LoadFromString()
    LLBC_Variant properties2;
    LLBC_ErrorAndReturnIfNot(LLBC_Properties::LoadFromString(content, properties2, &errMsg) == LLBC_OK,
                             LLBC_FAILED,
                             "LoadFromString failed, error:%s", errMsg.c_str());
    dumpProperties(properties2);

    // Test: LoadFromFile()
    LLBC_Variant properties3;
    const LLBC_String propFile = "test_prop.properties";
	std::cout <<"Load from file: " <<propFile <<std::endl;
    LLBC_ErrorAndReturnIfNot(LLBC_Properties::LoadFromFile(propFile, properties3, &errMsg) == LLBC_OK,
                             LLBC_FAILED,
                             "LoadFromFile failed, error:%s", errMsg.c_str());
    std::cout <<"Load from file <" <<propFile <<">, properties is:" <<std::endl;
	dumpProperties(properties3);

    // Test: SaveToFile()
    auto outPropFile = LLBC_Directory::SplitExt(propFile)[0] + "_out" + LLBC_Directory::SplitExt(propFile)[1];
    std::cout <<"Save to file: " <<outPropFile <<std::endl;
    LLBC_ErrorAndReturnIfNot(LLBC_Properties::SaveToFile(properties3, outPropFile, &errMsg) == LLBC_OK,
                             LLBC_FAILED,
                             "SveToFile failed, error:%s", errMsg.c_str());

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}
