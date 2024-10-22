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

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Common.h"

#include "src/common/_CommonMethods.h"

void pyllbc_InitCommon()
{
    LLBC_AddCustomErrno(PYLLBC_ERROR_COMMON, "pyllbc library error");
}

void pyllbc_DestroyCommon()
{
    LLBC_RemoveCustomErrno(PYLLBC_ERROR_COMMON);
}

void pyllbc_AddCommonMethods()
{
    pyllbc_CommonMethods &methods = *pyllbc_s_CommonMethods;
    pyllbc_TopModule->AddMethod(methods.GetVersionInfo);
}

void pyllbc_AddCommonObjs()
{
    pyllbc_Module *top = pyllbc_TopModule;
    top->AddSubModule(pyllbc_InlModule);

    /* Add exception type. */
    LLBC_String excName;
    excName.format("%s.error", PYLLBC_MOD_NAME);
    pyllbc_Exception = PyErr_NewException(const_cast<char *>(excName.c_str()), nullptr, nullptr);

    Py_INCREF(pyllbc_Exception);
    top->AddObject("error", pyllbc_Exception);

    /* Add secure output function. */
    pyllbc_CommonMethods &methods = *pyllbc_s_CommonMethods;
    top->AddMethod(methods.output);

    /* Add version info. */
    top->AddObject("MajorVersion", LLBC_majorVersion);
    top->AddObject("MinorVersion", LLBC_minorVersion);
    top->AddObject("UpdateNo", LLBC_updateNo);
    top->AddObject("Debug", LLBC_isDebugVer);
    top->AddObject("VerInfo", LLBC_GetVersionInfo(false));
    top->AddObject("VerboseVerInfo", LLBC_GetVersionInfo(true));

    /* Add endian module. */
    pyllbc_Module *endianMod = new pyllbc_Module("Endian", top);
    endianMod->AddObject("BigEndian", LLBC_Endian::BigEndian);
    endianMod->AddObject("NetEndian", LLBC_Endian::NetEndian);
    endianMod->AddObject("LittleEndian", LLBC_Endian::LittleEndian);
    endianMod->AddObject("MachineEndian", LLBC_MachineEndian);

    /* Add PyStream methods. */
    pyllbc_Module *inlMod = pyllbc_s_InlModule;
    inlMod->AddMethod(methods.Stream_New);
    inlMod->AddMethod(methods.Stream_Del);
    inlMod->AddMethod(methods.Stream_GetEndian);
    inlMod->AddMethod(methods.Stream_SetEndian);
    inlMod->AddMethod(methods.Stream_GetReadPos);
    inlMod->AddMethod(methods.Stream_SetReadPos);
    inlMod->AddMethod(methods.Stream_GetWritePos);
    inlMod->AddMethod(methods.Stream_SetWritePos);
    inlMod->AddMethod(methods.Stream_GetCap);
    inlMod->AddMethod(methods.Stream_SetCap);
    inlMod->AddMethod(methods.Stream_GetRaw);
    inlMod->AddMethod(methods.Stream_SetRaw);
    inlMod->AddMethod(methods.Stream_FmtRead);
    inlMod->AddMethod(methods.Stream_Read);
    inlMod->AddMethod(methods.Stream_Read_None);
    inlMod->AddMethod(methods.Stream_Read_Byte);
    inlMod->AddMethod(methods.Stream_Read_Bool);
    inlMod->AddMethod(methods.Stream_Read_Int16);
    inlMod->AddMethod(methods.Stream_Read_Int32);
    inlMod->AddMethod(methods.Stream_Read_Int64);
    inlMod->AddMethod(methods.Stream_Read_Float);
    inlMod->AddMethod(methods.Stream_Read_Double);
    inlMod->AddMethod(methods.Stream_Read_Str);
    inlMod->AddMethod(methods.Stream_Read_Str2);
    inlMod->AddMethod(methods.Stream_Read_Unicode);
    inlMod->AddMethod(methods.Stream_Read_ByteArray);
    inlMod->AddMethod(methods.Stream_Read_Buffer);
    inlMod->AddMethod(methods.Stream_Read_Stream);
    inlMod->AddMethod(methods.Stream_FmtWrite);
    inlMod->AddMethod(methods.Stream_Write);
    inlMod->AddMethod(methods.Stream_Write_None);
    inlMod->AddMethod(methods.Stream_Write_Byte);
    inlMod->AddMethod(methods.Stream_Write_Bool);
    inlMod->AddMethod(methods.Stream_Write_Int16);
    inlMod->AddMethod(methods.Stream_Write_Int32);
    inlMod->AddMethod(methods.Stream_Write_Int64);
    inlMod->AddMethod(methods.Stream_Write_Float);
    inlMod->AddMethod(methods.Stream_Write_Double);
    inlMod->AddMethod(methods.Stream_Write_Str);
    inlMod->AddMethod(methods.Stream_Write_Str2);
    inlMod->AddMethod(methods.Stream_Write_Unicode);
    inlMod->AddMethod(methods.Stream_Write_ByteArray);
    inlMod->AddMethod(methods.Stream_Write_Buffer);
    inlMod->AddMethod(methods.Stream_Write_Tuple);
    inlMod->AddMethod(methods.Stream_Write_List);
    inlMod->AddMethod(methods.Stream_Write_Sequence);
    inlMod->AddMethod(methods.Stream_Write_Dict);
    inlMod->AddMethod(methods.Stream_Write_Stream);
    inlMod->AddMethod(methods.Stream_EncodeSelf);
    inlMod->AddMethod(methods.Stream_GetCachedCompiledExprSize);
    inlMod->AddMethod(methods.Stream_GetCompiledExprCacheLimit);
    inlMod->AddMethod(methods.Stream_SetCompiledExprCacheLimit);
    inlMod->AddMethod(methods.Stream_IsExprCompiled);
    inlMod->AddMethod(methods.Stream_UncacheCompiledExpr);
    inlMod->AddMethod(methods.Stream_UncacheAllCompiledExprs);

    /* Add test data. */
    #ifdef LLBC_DEBUG
    pyllbc_Module *testMod = new pyllbc_Module("test", top);
    testMod->AddObject("boolVal", true);
    testMod->AddObject("charVal", 'A');
    testMod->AddObject("sint32Val", -32);
    testMod->AddObject("uint32Val", 32);
    testMod->AddObject("sint64Val", static_cast<sint64>(-64));
    testMod->AddObject("uint64Val", static_cast<uint64>(64));
    testMod->AddObject("doubleVal", 3.14159);
    testMod->AddObject("stringVal", "Hello pyllbc");
    #endif
}
