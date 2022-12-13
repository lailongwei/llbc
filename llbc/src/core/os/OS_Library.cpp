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


#include "llbc/common/Export.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <dlfcn.h>
#endif // Non-Win32

#include "llbc/core/file/Directory.h"

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
#else // LLBC_TARGET_PLATFORM_WIN32
    // Note, WIN32 API ::GetProcAddress not support slashes(/), so replace it.
    LLBC_String libName(fileName);
    libName.findreplace(LLBC_SLASH_A, LLBC_BACKLASH_A);
    if (libName.empty())
        libName = LLBC_Directory::ModuleFilePath();

    if ((handle = ::LoadLibraryExA(libName.c_str(), nullptr, 0)) == LLBC_INVALID_LIBRARY_HANDLE)
        LLBC_SetLastError(LLBC_ERROR_OSAPI);

    return handle;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_LibraryFun LLBC_GetProcAddress(LLBC_LibraryHandle handle, const char *procName)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    // Clear any existing error.
    dlerror();

    void *proc = dlsym(handle, procName);
    if (dlerror() != nullptr)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return nullptr;
    }

    if (!proc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
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
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (::FreeLibrary(handle) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

__LLBC_NS_END
