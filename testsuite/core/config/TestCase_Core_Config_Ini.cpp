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
    LLBC_PrintLn("LLBC_Ini test:");

    LLBC_Ini ini;
    const LLBC_String iniFile = "IniTestCfg.ini";

    // Load ini file.
    LLBC_PrintLn("Load ini file: %s", iniFile.c_str());
    int ret = ini.LoadFromFile(iniFile);
    if (ret != LLBC_OK)
    {
        LLBC_PrintLn("Failed to load ini config file, error: %s", LLBC_FormatLastError());
        LLBC_PrintLn("Ini parse error: %s", ini.GetLoadError().c_str());
        getchar();

        return LLBC_FAILED;
    }
    LLBC_PrintLn("Success to load ini file");

    LLBC_Ini iniToo = ini;
    LLBC_Ini iniTooToo;
    iniTooToo = ini;

    // Test size_t/long/ulong
    LLBC_PrintLn("Test long/ulong/size_t value read:");
    long lVal = ini.GetValue<long>("Long_ULong_Size_t", "lVal", 3);
    ulong ulVal = ini.GetValue<ulong>("Long_ULong_Size_t", "ulVal", 4);
    size_t sizeVal = ini.GetValue<size_t>("Long_ULong_Size_t", "size_tVal", 5);
    LLBC_PrintLn("longVal = %ld, ulongVal = %lu, size_tVal = %lu", lVal, ulVal, sizeVal);

    // Output all section names.
    LLBC_PrintLn("All section names:");
    const std::map<LLBC_String, LLBC_IniSection *> &sections = ini.GetAllSections();
    for (std::map<LLBC_String, LLBC_IniSection *>::const_iterator it = sections.begin();
         it != sections.end();
         it++)
        LLBC_PrintLn("    %s", it->first.c_str());

    // Get some config value.
    LLBC_PrintLn("Config [Hello] Cfg1 = %s, comment: %s", 
        ini.GetValue<LLBC_String>("Hello", "Cfg1").c_str(), ini.GetComment("Hello", "Cfg1").c_str());
    LLBC_PrintLn("Config [Hello] Cfg3 = %s", ini.GetValue<LLBC_String>("Hello", "Cfg3").c_str());

    LLBC_PrintLn("Config [World] Cfg1 = %s", ini.GetValue<LLBC_String>("World", "Cfg1").c_str());
    LLBC_PrintLn("Config [World] Cfg3 = %s", ini.GetValue<LLBC_String>("World", "Cfg3").c_str());

    // Write some new ini config to ini object.
    LLBC_IniSection newSection;
    newSection.SetValue("Cfg1", 30, "The new config");
    ini.SetSection("NewSection", newSection);
    LLBC_PrintLn("Config [NewSection] Cfg1 = %d", ini.GetValue<int>("NewSection", "Cfg1"));

    // Write ini config to file.
    const LLBC_String savedFile = "test_ini_saved.ini";
    LLBC_PrintLn("Save ini config to file: %s", savedFile.c_str());
    if (ini.SaveToFile(savedFile)!= LLBC_OK)
    {
        LLBC_PrintLn(
            "Save ini config to %s failed, error: %s", savedFile.c_str(), LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}
