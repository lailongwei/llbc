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

#pragma once

#include "pyllbc/common/LibHeader.h"

/* the pyllbc debug macro define. */
#ifdef LLBC_DEBUG
# define PYLLBC_DEBUG
#endif // defined LLBC_DEBUG macro.

/* The pyllbc some common module names define. */
#ifdef PYLLBC_DEBUG 
# define PYLLBC_MOD_NAME        "llbc"
# define PYLLBC_INL_MOD_NAME    "inl"
#else // undefine PYLLBC_DEBUG macro.
# define PYLLBC_MOD_NAME        "llbc"
# define PYLLBC_INL_MOD_NAME    "inl"
#endif // defined PYLLBC_DEBUG macro.

/* Module init func define. */
#if LLBC_TARGET_PLATFORM_WIN32
 #define PYLLBC_MOD_INIT_FUNC   LLBC_EXTERN_C __declspec(dllexport)
#elif LLBC_TARGET_PLATFORM_LINUX
 #define PYLLBC_MOD_INIT_FUNC   LLBC_EXTERN_C __attribute__ ((__visibility__("default")))
#elif LLBC_TARGET_PLATFORM_MAC
 #define PYLLBC_MOD_INIT_FUNC   LLBC_EXTERN_C __attribute__ ((__visibility__("default")))
#elif LLBC_TARGET_PLATFORM_IPHONE
 #define PYLLBC_MOD_INIT_FUNC   LLBC_EXTERN_C __attribute__ ((__visibility__("default")))
#elif LLBC_TARGET_PLATFORM_ANDROID
 #define PYLLBC_MOD_INIT_FUNC   LLBC_EXTERN_C __attribute__ ((__visibility__("default")))
#else
 #error "Unknown platform, could not compile pyllbc library!"
#endif

/* Some build-in python class macros define. */
#define PYLLBC_NONE_CLS         (reinterpret_cast<PyObject *>(Py_TYPE(Py_None)))
#define PYLLBC_BOOL_CLS         (reinterpret_cast<PyObject *>(&PyBool_Type))
#define PYLLBC_INT_CLS          (reinterpret_cast<PyObject *>(&PyInt_Type))
#define PYLLBC_LONG_CLS         (reinterpret_cast<PyObject *>(&PyLong_Type))
#define PYLLBC_FLOAT_CLS        (reinterpret_cast<PyObject *>(&PyFloat_Type))
#define PYLLBC_STR_CLS          (reinterpret_cast<PyObject *>(&PyString_Type))
#define PYLLBC_UNICODE_CLS      (reinterpret_cast<PyObject *>(&PyUnicode_Type))
#define PYLLBC_BYTEARRAY_CLS    (reinterpret_cast<PyObject *>(&PyByteArray_Type))
#define PYLLBC_BUFFER_CLS       (reinterpret_cast<PyObject *>(&PyBuffer_Type))
#define PYLLBC_TUPLE_CLS        (reinterpret_cast<PyObject *>(&PyTuple_Type))
#define PYLLBC_LIST_CLS         (reinterpret_cast<PyObject *>(&PyList_Type))
#define PYLLBC_DICT_CLS         (reinterpret_cast<PyObject *>(&PyDict_Type))

/* Top/Inl modules macro define. */
#define pyllbc_TopModule        pyllbc_s_TopModule
#define pyllbc_InlModule        pyllbc_s_InlModule


