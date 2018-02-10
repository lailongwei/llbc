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


#include "core/config/TestCase_Core_Config_Ini.h"

TestCase_Core_Config_Ini::TestCase_Core_Config_Ini()
{
}

TestCase_Core_Config_Ini::~TestCase_Core_Config_Ini()
{
}

int TestCase_Core_Config_Ini::Run(int argc, char *argv[])
{
    LLBC_PrintLine("LLBC_Ini test:");

    LLBC_Ini ini;
    const LLBC_String iniFile = "test_ini.ini";

    // Load ini file.
    LLBC_PrintLine("Load ini file: %s", iniFile.c_str());
    int ret = ini.LoadFromFile(iniFile);
    if (ret != LLBC_OK)
    {
        LLBC_PrintLine("Failed to load ini config file, error: %s", LLBC_FormatLastError());
        LLBC_PrintLine("Ini parse error: %s", ini.GetLoadError().c_str());
        getchar();

        return LLBC_FAILED;
    }
    LLBC_PrintLine("Success to load ini file");

    // Output all section names.
    LLBC_PrintLine("All section names:");
    const std::map<LLBC_String, LLBC_IniSection *> &sections = ini.GetAllSections();
    for (std::map<LLBC_String, LLBC_IniSection *>::const_iterator it = sections.begin();
         it != sections.end();
         it++)
        LLBC_PrintLine("    %s", it->first.c_str());

    // Get some config value.
    LLBC_PrintLine("Config [Hello] Cfg1 = %s, comment: %s", 
        ini.GetValue<LLBC_String>("Hello", "Cfg1").c_str(), ini.GetComment("Hello", "Cfg1").c_str());
    LLBC_PrintLine("Config [Hello] Cfg3 = %s", ini.GetValue<LLBC_String>("Hello", "Cfg3").c_str());

    LLBC_PrintLine("Config [World] Cfg1 = %s", ini.GetValue<LLBC_String>("World", "Cfg1").c_str());
    LLBC_PrintLine("Config [World] Cfg3 = %s", ini.GetValue<LLBC_String>("World", "Cfg3").c_str());

    // Write some new ini config to ini object.
    LLBC_IniSection newSection;
    newSection.SetValue("Cfg1", 30, "The new config");
    ini.SetSection("NewSection", newSection);
    LLBC_PrintLine("Config [NewSection] Cfg1 = %d", ini.GetValue<int>("NewSection", "Cfg1"));

    // Write ini config to file.
    const LLBC_String savedFile = "test_ini_saved.ini";
    LLBC_PrintLine("Save ini config to file: %s", savedFile.c_str());
    if (ini.SaveToFile(savedFile)!= LLBC_OK)
    {
        LLBC_PrintLine(
            "Save ini config to %s failed, error: %s", savedFile.c_str(), LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}