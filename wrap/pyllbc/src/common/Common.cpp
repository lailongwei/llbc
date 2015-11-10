/**
 * @file    Common.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief
 */

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
    pyllbc_Exception = PyErr_NewException(const_cast<char *>(excName.c_str()), NULL, NULL);

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
    endianMod->AddObject("MachineEndian", LLBC_GetMachineEndianType());

    /* Add PyStream methods. */
    pyllbc_Module *inlMod = pyllbc_s_InlModule;
    inlMod->AddMethod(methods.NewPyStream);
    inlMod->AddMethod(methods.DelPyStream);
    inlMod->AddMethod(methods.GetPyStreamEndian);
    inlMod->AddMethod(methods.SetPyStreamEndian);
    inlMod->AddMethod(methods.GetPyStreamPos);
    inlMod->AddMethod(methods.SetPyStreamPos);
    inlMod->AddMethod(methods.GetPyStreamSize);
    inlMod->AddMethod(methods.SetPyStreamSize);
    inlMod->AddMethod(methods.PyStreamGetRaw);
    inlMod->AddMethod(methods.PyStreamSetRaw);
    inlMod->AddMethod(methods.PyStreamRead);
    inlMod->AddMethod(methods.PyStreamRead_None);
    inlMod->AddMethod(methods.PyStreamRead_Byte);
    inlMod->AddMethod(methods.PyStreamRead_Bool);
    inlMod->AddMethod(methods.PyStreamRead_Int16);
    inlMod->AddMethod(methods.PyStreamRead_Int32);
    inlMod->AddMethod(methods.PyStreamRead_Int64);
    inlMod->AddMethod(methods.PyStreamRead_Float);
    inlMod->AddMethod(methods.PyStreamRead_Double);
    inlMod->AddMethod(methods.PyStreamRead_PyInt);
    inlMod->AddMethod(methods.PyStreamRead_PyLong);
    inlMod->AddMethod(methods.PyStreamRead_Str);
    inlMod->AddMethod(methods.PyStreamRead_Str2);
    inlMod->AddMethod(methods.PyStreamRead_Unicode);
    inlMod->AddMethod(methods.PyStreamRead_ByteArray);
    inlMod->AddMethod(methods.PyStreamRead_Buffer);
    inlMod->AddMethod(methods.PyStreamRead_Stream);
    inlMod->AddMethod(methods.PyStreamFmtRead);
    inlMod->AddMethod(methods.PyStreamWrite);
    inlMod->AddMethod(methods.PyStreamWrite_None);
    inlMod->AddMethod(methods.PyStreamWrite_Byte);
    inlMod->AddMethod(methods.PyStreamWrite_Bool);
    inlMod->AddMethod(methods.PyStreamWrite_Int16);
    inlMod->AddMethod(methods.PyStreamWrite_Int32);
    inlMod->AddMethod(methods.PyStreamWrite_Int64);
    inlMod->AddMethod(methods.PyStreamWrite_Float);
    inlMod->AddMethod(methods.PyStreamWrite_Double);
    inlMod->AddMethod(methods.PyStreamWrite_PyInt);
    inlMod->AddMethod(methods.PyStreamWrite_PyLong);
    inlMod->AddMethod(methods.PyStreamWrite_Str);
    inlMod->AddMethod(methods.PyStreamWrite_Str2);
    inlMod->AddMethod(methods.PyStreamWrite_Unicode);
    inlMod->AddMethod(methods.PyStreamWrite_ByteArray);
    inlMod->AddMethod(methods.PyStreamWrite_Buffer);
    inlMod->AddMethod(methods.PyStreamWrite_Tuple);
    inlMod->AddMethod(methods.PyStreamWrite_List);
    inlMod->AddMethod(methods.PyStreamWrite_Sequence);
    inlMod->AddMethod(methods.PyStreamWrite_Dict);
    inlMod->AddMethod(methods.PyStreamWrite_Stream);
    inlMod->AddMethod(methods.PyStreamFmtWrite);
    inlMod->AddMethod(methods.PyStreamEncodeSelf);
    inlMod->AddMethod(methods.PyStreamGetCachedSize);
    inlMod->AddMethod(methods.PyStreamIsExprCompiled);
    inlMod->AddMethod(methods.PyStreamGetCacheLimit);
    inlMod->AddMethod(methods.PyStreamSetCacheLimit);
    inlMod->AddMethod(methods.PyStreamDiscardExpr);
    inlMod->AddMethod(methods.PyStreamDiscardAllExprs);

    /* Add test data. */
    pyllbc_Module *testMod = new pyllbc_Module("test", top);
    testMod->AddObject("boolVal", true);
    testMod->AddObject("charVal", 'A');
    testMod->AddObject("sint32Val", -32);
    testMod->AddObject("uint32Val", 32);
    testMod->AddObject("sint64Val", static_cast<sint64>(-64));
    testMod->AddObject("uint64Val", static_cast<uint64>(64));
    testMod->AddObject("doubleVal", 3.14159);
    testMod->AddObject("stringVal", "Hello pyllbc");
}
