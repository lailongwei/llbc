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


#include "core/config/TestCase_Core_Config_Property.h"

TestCase_Core_Config_Property::TestCase_Core_Config_Property()
{
#if LLBC_TARGET_PLATFORM_IPHONE
    _file = LLBC_Directory::Join(LLBC_Directory::TempDir(), "test_prop.cfg");
#else
    _file = LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), "test_prop.cfg");
#endif
}

TestCase_Core_Config_Property::~TestCase_Core_Config_Property()
{
    LLBC_File::DeleteFile(_file);
}

int TestCase_Core_Config_Property::Run(int argc, char *argv[])
{
    std::cout <<"LLBC_Property test: " <<std::endl;

	// Test SetValue()/SaveToContent().
	LLBC_Property cfg;
	std::cout <<"Basic test, add some properties:" <<std::endl;
	cfg.SetValue("llbc.test.intVal", 30, "The intege r value");
	cfg.SetValue("llbc.test.boolFalse", false, "The bool false value");
	cfg.SetValue("llbc.test.boolTrue", true); // Not add comments.
	cfg.SetValue("llbc.test.strVal", "Hello #World!", "The string value ######\\#\\#");

	LLBC_String content;
	cfg.SaveToContent(content);
	std::cout <<"While set, content is :" <<std::endl;
 	std::cout <<content <<std::endl;
	std::cout <<"  Fetch llbc.test.intVal: " <<cfg.GetValue("llbc.test.intVal").ValueToString() <<std::endl;
	std::cout <<"  Fetch llbc.test.boolFalse: " <<cfg.GetValue("llbc.test.boolFalse").ValueToString() <<std::endl;
	std::cout <<"  Fetch llbc.test.boolTrue: " <<cfg.GetValue("llbc.test.boolTrue").ValueToString() <<std::endl;
	std::cout <<"  Fetch llbc.test.strVal: " <<cfg.GetValue("llbc.test.strVal").ValueToString() <<std::endl;

	// Test GetProperty()/GetName()/GetPropertyNames()
	const LLBC_Property *subCfg = cfg.GetProperty("llbc.test.intVal");
	if (!subCfg)
	{
		std::cerr <<"Could not get the property, test failed" <<std::endl;

		getchar();
		return 0;
	}
	const LLBC_String propName = subCfg->GetName();
	std::cout <<"Property 'llbc.test.intVal' name is: " <<propName <<std::endl;

	LLBC_Strings names = cfg.GetPropertyNames(false);
	std::cout <<"Get property names(non nest):" <<std::endl;
	for (LLBC_Strings::const_iterator it = names.begin();
		it != names.end();
		it++)
		std::cout <<"  " <<*it <<std::endl;

	names = cfg.GetPropertyNames(true);
	std::cout <<"Get property names(nest):" <<std::endl;
	for (LLBC_Strings::const_iterator it = names.begin();
		 it != names.end();
		 it++)
		 std::cout <<"  " <<*it <<std::endl;

	// Test if a leaf property changed to non-leaf property, it's value exist or not?
	std::cout <<"Add sub path to llbc.test.intVal:" <<std::endl;
	cfg.SetValue("llbc.test.intVal.subVal", 10086, "Modified value!");
	std::cout <<"After add llbc.test.intVal value: " <<cfg.GetValue("llbc.test.intVal").ValueToString() <<std::endl;

	// Test Delete property.
	std::cout <<"Delete property'llbc.test.intVal':" <<std::endl;
	cfg.RemoveProperty("llbc.test.intVal.subVal", false);
	std::cout <<"After delete, property'llbc.test.intVal.subVal' exist? "
		<<(cfg.HasProperty("llbc.test.intVal.subVal") ? "true" : "false") <<std::endl;
	std::cout <<"After delete, property'llbc.test.intVal' value: "
		<<cfg.GetValue("llbc.test.intVal").ValueToString() <<std::endl;

	// Test SaveToFile
	std::cout <<"Write content to file: " <<_file <<std::endl;
    cfg.SaveToFile(_file);
	std::cout <<"Write done!" <<std::endl;

    // Test LoadFromFile
    // Load invalid file path.
    const LLBC_String invalidFilePath = "asldkjfaslf2l323lkjl;aj;afaj;lfkjslfk.a";
    if (cfg.LoadFromFile(invalidFilePath) != LLBC_OK)
    {
        std::cerr <<"Load from file failed, right!" <<std::endl;
        std::cerr <<"Last error: " <<LLBC_FormatLastError() <<std::endl;
        std::cerr <<"The invalid file path: " <<invalidFilePath <<std::endl;

        std::cout <<"The 'llbc.test.strVal' value is: " 
            <<cfg.GetValue("llbc.test.strVal").ValueToString() <<std::endl;
    }

    // Load right path.
    if (cfg.LoadFromFile(_file) != LLBC_OK)
    {
        std::cerr <<"Load from file failed, error: " <<LLBC_FormatLastError() <<std::endl;
        std::cerr <<"Verbose error: " <<cfg.GetLoadErrorDesc() <<std::endl;
        std::cerr <<"Check it!" <<std::endl;

        getchar();
        return 0;
    }

    // Try write error format line to test file.
    std::cout <<"Try write error format line to property content" <<std::endl;
    cfg.SaveToContent(content);
    if (cfg.LoadFromContent(content + "llbc.test.$errorProp 3434 # hey") != LLBC_OK)
    {
        std::cout <<"Load failed, error: " <<LLBC_FormatLastError() <<std::endl;
        std::cout <<"Verbose error: " <<cfg.GetLoadErrorDesc() <<std::endl;
    }
    else
    {
        std::cerr <<"Load from content success, but content format error, check it!" <<std::endl;
        
        getchar();
        return 0;
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}
