/**
 * @file    OS_Library.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/01
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/file/Directory.h"

#include "llbc/core/os/OS_Directory.h"
#include "llbc/core/os/OS_Library.h"

__LLBC_INTERNAL_NS_BEGIN

static void __LLBC_SelfLibAttach()
{
    // Do nothing.
}

static void __LLBC_SelfLibDetach()
{
    // Do nothing.
}

__LLBC_INTERNAL_NS_END

/** Library Dll attach/detach event handler function(s) encapsulation */
#if LLBC_TARGET_PLATFORM_NON_WIN32

extern "C" __attribute__((constructor)) void LLBC_SelfLibAttach()
{
    LLBC_INTERNAL_NS __LLBC_SelfLibAttach();
}

extern "C" __attribute__((destructor)) void LLBC_SelfLibDetach()
{
    LLBC_INTERNAL_NS __LLBC_SelfLibDetach();
}

#else // LLBC_TARGET_PLATFORM_NON_WIN32

// extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad)
// {
//    switch(fdwReason)
//  {
//  case DLL_PROCESS_ATTACH:
//    // In llbc library, we don't care thread attach/detach events, so, disable it.
//      ::DisableThreadLibraryCalls(hinstDll);
//      LLBC_INTERNAL_NS __LLBC_SelfLibAttach();
//      break;
//
//  case DLL_THREAD_ATTACH:
//  case DLL_THREAD_DETACH:
//      ASSERT(false && "llbc library fatal error occurred, thread attach/detach dll event received!");
//      break;
//
//  case DLL_PROCESS_DETACH:
//      LLBC_INTERNAL_NS __LLBC_SelfLibDetach();
//      break;
//  }
//
//  //! Only DLL_PROCESS_ATTACH event care this return code. other events return code, operation system will ignore.
//  return TRUE;
// }

#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

LLBC_LibraryHandle LLBC_LoadLibrary(const char *fileName)
{
    LLBC_LibraryHandle handle = LLBC_INVALID_LIBRARY_HANDLE;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if ((handle = dlopen(fileName, RTLD_LAZY)) == LLBC_INVALID_LIBRARY_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
    }

    return handle;
#else // LLBC_TARGET_PLATFORM_NON_WIN32
    // Note, WIN32 API ::GetProcAddress not support slashes(/), so replace it.
    LLBC_String libName(fileName);
    LLBC_String::size_type pos = libName.find(LLBC_SLASH_A);
    for(; pos != LLBC_String::npos; pos = libName.find(LLBC_SLASH_A))
    {
        libName.replace(pos, 1, 1, LLBC_BACKLASH_A);
    }

    if (libName.empty())
    {
        libName = LLBC_GetModuleFileName();
    }

    if ((handle = ::LoadLibraryExA(libName.c_str(), NULL, 0)) == LLBC_INVALID_LIBRARY_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
    }

    return handle;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

LLBC_LibraryFun LLBC_GetProcAddress(LLBC_LibraryHandle handle, const char *procName)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    // Clear any existing error.
    dlerror();

    void *proc = dlsym(handle, procName);
    if (dlerror() != NULL)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return NULL;
    }

    if (!proc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return (LLBC_LibraryFun)proc;
#else // LLBC_TARGET_PLATFORM_NON_WIN32
    FARPROC proc = ::GetProcAddress(handle, procName);
    if (!proc)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
    }

    return (LLBC_LibraryFun)proc;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

int LLBC_CloseLibrary(LLBC_LibraryHandle handle)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (dlclose(handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (::FreeLibrary(handle) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
