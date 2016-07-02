/**
 * @file    Core.cpp
 * @author  Longwei Lai<lailongwei@126>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/core/Core.h"

#include "src/core/_CoreMethods.h"

void pyllbc_InitCore()
{
}

void pyllbc_DestroyCore()
{
}

void pyllbc_AddCoreMethods()
{
    pyllbc_Module *inl = pyllbc_InlModule;

    // core/guid
    pyllbc_CoreMethods &methods = *pyllbc_s_CoreMethods;
    inl->AddMethod(methods.GenGUID);

    // core/file
    pyllbc_Module *top = pyllbc_TopModule;
    top->AddMethod(methods.GetModuleFileName);
    top->AddMethod("get_module_filename",
                   methods.GetModuleFileName.ml_meth,
                   methods.GetModuleFileName.ml_flags,
                   methods.GetModuleFileName.ml_doc);

    // core/bundle
    inl->AddMethod(methods.NewBundle);
    inl->AddMethod(methods.DelBundle);
    inl->AddMethod(methods.GetMainBundle);
    inl->AddMethod(methods.GetBundlePath);
    inl->AddMethod(methods.GetBundleName);
    inl->AddMethod(methods.InitBundle);
    inl->AddMethod(methods.GetBundleResPath);

    // core/random
    pyllbc_Module *randMod = LLBC_New2(pyllbc_Module, "Random", top);
    randMod->AddMethod(methods.seed);
    randMod->AddMethod(methods.rand);
    randMod->AddMethod(methods.rand53real);
    randMod->AddMethod(methods.choice);
    randMod->AddMethod(methods.shuffle);
    randMod->AddMethod(methods.booljudge);

    // core/config
    inl->AddMethod(methods.Property_New);
    inl->AddMethod(methods.Property_Delete);
    inl->AddMethod(methods.Property_FromContent);
    inl->AddMethod(methods.Property_FromFile);
    inl->AddMethod(methods.Property_ToContent);
    inl->AddMethod(methods.Property_ToFile);
    inl->AddMethod(methods.Property_GetPropertyCount);
    inl->AddMethod(methods.Property_GetPropertyNames);
    inl->AddMethod(methods.Property_GetValue);
    inl->AddMethod(methods.Property_SetValue);
    inl->AddMethod(methods.Property_GetComments);
    inl->AddMethod(methods.Property_SetComments);
    inl->AddMethod(methods.Property_HasProperty);
    inl->AddMethod(methods.Property_GetProperty);
    inl->AddMethod(methods.Property_RemoveProperty);
    inl->AddMethod(methods.Property_RemoveAllPropertyes);

    // core/log
    inl->AddMethod(methods.InitLoggerMgr);
    inl->AddMethod(methods.LogMsg);

    // core/utils
    top->AddMethod(methods.refcnt);
    inl->AddMethod(methods.HashString);
}

void pyllbc_AddCoreObjs()
{
}
