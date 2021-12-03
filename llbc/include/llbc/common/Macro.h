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

#ifndef __LLBC_COM_MACRO_H__
#define __LLBC_COM_MACRO_H__

#include "llbc/common/PFConfig.h"

// The llbc namespace define.
#define LLBC_NAMESPACE ::llbc::
#define LLBC_NS        LLBC_NAMESPACE

// The llbc internal namespace  define.
#define LLBC_INTERNAL_NAMESPACE ::__llbc::
#define LLBC_INTERNAL_NS        LLBC_INTERNAL_NAMESPACE
#define LLBC_INL_NS             LLBC_INTERNAL_NAMESPACE

// llbc library namespace macros(begin/end) define.
#define __LLBC_NS_BEGIN \
    namespace llbc {    \

#define __LLBC_NS_END   \
    }                   \

// llbc library internal namespace macros(begin/end) define.
#define __LLBC_INTERNAL_NS_BEGIN \
    namespace __llbc {           \

#define __LLBC_INTERNAL_NS_END   \
    }                            \

// llbc library error describe buffer size.
#define __LLBC_ERROR_DESC_SIZE    2048

// Debug macro define.
#ifdef DEBUG
 #ifndef LLBC_DEBUG
  #define LLBC_DEBUG 1
 #endif
#else
 #ifdef LLBC_DEBUG
  #error "Defined LLBC_DEBUG macro, but DEBUG macro undefined!"
 #endif
#endif

// NDebug/Release macro define.
#ifndef LLBC_DEBUG
 #define LLBC_NDEBUG 1
 #define LLBC_RELEASE 1
#endif

// Exception throw macro define.
#define LLBC_THROW(e, s) throw e(s)

// The likely & unlikely builtin expect macro.
#ifdef __GNUC__
 #ifndef LIKELY
  #define LIKELY(x) __builtin_expect(!!(x), 1)
 #endif
 #ifndef UNLIKELY
  #define UNLIKELY(x) __builtin_expect(!!(x), 0)
 #endif
#else
 #ifndef LIKELY
  #define LIKELY(x) (x)
 #endif
 #ifndef UNLIKELY
  #define UNLIKELY(x) (x)
 #endif
#endif // __GNUC__

// Min / Max macro define.
#ifndef MIN
#define MIN(x, y)           \
    ((x) < (y) ? (x) : (y)) \

#endif

#ifndef MAX
#define MAX(x, y)           \
    ((x) > (y) ? (x) : (y)) \

#endif

// ISUNSIGNED macro define.
#define LLBC_ISUNSIGNED_TYPE(type)     ((type)(0 - 1) > 0)
#define LLBC_ISUNSIGNED_VAL(type, val) ((type)val >= 0 && (type)~a >= (type)0)

// Force inline macro define.
#if defined(_MSC_VER)
 #define LLBC_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
 #define LLBC_FORCE_INLINE __inline__ __attribute__((always_inline))
#else
 #define LLBC_FORCE_INLINE inline
#endif

// Extern macro define.
#ifdef __cplusplus
 #define LLBC_EXTERN        extern
 #define LLBC_EXTERN_C      extern "C"
#else
 #define LLBC_EXTERN        extern
 #define LLBC_EXTERN_C      
#endif

// Extern "C" macro define.
#ifdef __cplusplus
 #define LLBC_BEGIN_C_DECL extern "C" {
 #define LLBC_END_C_DECL }
#else
 #define LLBC_BEGIN_C_DECL
 #define LLBC_END_C_DECL
#endif

// The hidden macro define.
#if LLBC_TARGET_PLATFORM_LINUX
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN
 #endif
#elif LLBC_TARGET_PLATFORM_WIN32
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN
 #endif
#elif LLBC_TARGET_PLATFORM_IPHONE
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN
 #endif
#elif LLBC_TARGET_PLATFORM_MAC
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN
 #endif
#elif LLBC_TARGET_PLATFORM_ANDROID
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN
 #endif
#endif

// The dllexport macro define.
#if LLBC_TARGET_PLATFORM_LINUX
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 0
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
#elif LLBC_TARGET_PLATFORM_WIN32
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 0
  #define LLBC_EXPORT __declspec(dllimport)
 #endif
#elif LLBC_TARGET_PLATFORM_IPHONE
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
#elif LLBC_TARGET_PLATFORM_MAC
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 0
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
#elif LLBC_TARGET_PLATFORM_ANDROID
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 0
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
#endif

// ASSERT macro define.
#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

// Symbol concat macro define.
#define LLBC_Concat(x, y)  LLBC_IConcat(x, y)
#define LLBC_IConcat(x, y) x##y

// String concat macro define.
#define LLBC_ConcatStr(x, y)  LLBC_IConcatStr(x, y)
#define LLBC_IConcatStr(x, y) #x#y

// Define register keyword to empty n c++11(in c++11 standard, register is deprecated)
#ifdef LLBC_CPP11
#define register
#endif

// Disable assignments of objects.
#define LLBC_DISABLE_ASSIGNMENT(name)               \
private:                                            \
    name(const name &);                             \
    name &operator =(const name &)                  \

// Thread local macro define.
#if LLBC_TARGET_PLATFORM_LINUX
#define LLBC_THREAD_LOCAL __thread
#elif LLBC_TARGET_PLATFORM_WIN32
#define LLBC_THREAD_LOCAL __declspec(thread)
#elif LLBC_TARGET_PLATFORM_IPHONE
#define LLBC_THREAD_LOCAL __thread
#elif LLBC_TARGET_PLATFORM_MAC
#define LLBC_THREAD_LOCAL __thread
#elif LLBC_TARGET_PLATFORM_ANDROID
#define LLBC_THREAD_LOCAL __thread
#endif

// Deprecated attribute macro define.
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define LLBC_DEPRECATED __attribute__((deprecated))
#elif _MSC_VER >= 1400 // VS 2005 or higher.
#define LLBC_DEPRECATED __declspec(deprecated)
#else
#define LLBC_DEPRECATED
#endif

// Unused param macro.
#define LLBC_UNUSED_PARAM(param)  (void)param

// No except macro.
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #ifdef __clang__
  #if (__has_feature(cxx_noexcept))
   #define LLBC_NO_EXCEPT            noexcept
  #else // Has feature: cxx_noexcept
   #define LLBC_NO_EXCEPT            throw()
  #endif // Not exist feature: cxx_noexcept
 #else // defined __clang__
  #define LLBC_NO_EXCEPT            throw()
 #endif // undefined __clang__
#else // LLBC_TARGET_PLATFORM_NON_WIN32
 #define LLBC_NO_EXCEPT
#endif // LLBC_TARGET_PLATFORM_WIN32

// Defer macro define.

/* Memory operations macros. */
// allocate/reallocate/free.
#define LLBC_Malloc(type, size)             (reinterpret_cast<type *>(::malloc(size)))
#define LLBC_Calloc(type, size)             (reinterpret_cast<type *>(::calloc(size, 1)))
#define LLBC_Realloc(type, memblock, size)  (reinterpret_cast<type *>(::realloc((memblock), (size))))
#define LLBC_Free(memblock)                 (::free(memblock))
#define LLBC_XFree(memblock)        \
    do {                            \
        if (LIKELY(memblock)) {     \
            LLBC_Free(memblock);    \
            (memblock) = nullptr;   \
        }                           \
    } while(0)                      \

// new/delete.
#define LLBC_New(cls, ...)                  (new cls(__VA_ARGS__))
#define LLBC_News(cls, size)                (new cls[size])
#define LLBC_Delete(objptr)                 (delete (objptr))
#define LLBC_XDelete(objptr)        \
    do {                            \
        if (LIKELY(objptr)) {       \
            LLBC_Delete(objptr);    \
            (objptr) = nullptr;     \
        }                           \
    } while (0)                     \

#define LLBC_Deletes(objsptr)               (delete [](objsptr))
#define LLBC_XDeletes(objsptr)      \
    do {                            \
        if (LIKELY(objsptr)) {      \
            LLBC_Deletes(objsptr);  \
            objsptr = nullptr;      \
        }                           \
    } while(0)                      \

#define LLBC_Recycle(objptr)                LLBC_NS LLBC_PoolObjectReflection::Recycle(objptr)
#define LLBC_XRecycle(objptr)               LLBC_NS LLBC_PoolObjectReflection::RecycleX(objptr)

// memory set.
#define LLBC_MemSet(dst, c, count)          (::memset(dst, c, count))
// memory copy.
#define LLBC_MemCpy(dst, src, s)                                    \
    do {                                                            \
        size_t ___s = static_cast<size_t>(s);                       \
        char *___dst = reinterpret_cast<char *>(dst);               \
        const char *___src = reinterpret_cast<const char *>(src);   \
        if (UNLIKELY(!___dst || !___src || ___s <= 0))              \
        {                                                           \
            break;                                                  \
        }                                                           \
                                                                    \
        if (___dst + ___s <= ___src || ___dst >= ___src + ___s)     \
        {                                                           \
            memcpy(___dst, ___src, ___s);                           \
        }                                                           \
        else                                                        \
        {                                                           \
            char *buf = reinterpret_cast<                           \
                char *>(malloc(___s));                              \
            memcpy(buf, ___src, ___s);                              \
            memcpy(___dst, buf, ___s);                              \
            free(buf);                                              \
        }                                                           \
    } while (0)                                                     \

/**
 * Format argument.
 * @param[in] fmt  - the format string.
 * @param[out] buf - the formatted string, must call LLBC_Free to free memory.
 *                   if failed, this macro set retStr value to nullptr and set last error.
 * @param[out] len - the formatted string length, in bytes, not including tailing character.
 *                   this macro always filled the tailing character.
 */
#if LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_FormatArg __LLBC_FormatArg_WIN32
#else // LLBC_TARGET_PLATFORM_NON_WIN32
 #define LLBC_FormatArg __LLBC_FormatArg_NonWIN32
#endif // LLBC_TARGET_PLATFORM_WIN32

/**
 * WIN32 specified internal macro, use to format string.
 */
#if LLBC_TARGET_PLATFORM_WIN32
#define __LLBC_FormatArg_WIN32(fmt, buf, len)                                    \
    do {                                                                         \
        int &___len = (len);                                                     \
        char *&___buf = (buf);                                                   \
                                                                                 \
        if (UNLIKELY(!(fmt))) {                                                  \
            ___len = 0; ___buf = nullptr;                                        \
            LLBC_SetLastError(LLBC_ERROR_INVALID);                               \
            break;                                                               \
        }                                                                        \
                                                                                 \
        va_list ___ap;                                                           \
                                                                                 \
        int ___bufSize = 1024; ___len = 0;                                       \
        ___buf = LLBC_Malloc(char, ___bufSize + 1);                              \
        while (true) {                                                           \
            va_start(___ap, fmt);                                                \
            ___len = ::vsnprintf_s(___buf, ___bufSize, _TRUNCATE, (fmt), ___ap); \
            va_end(___ap);                                                       \
                                                                                 \
            if (___len >= 0)                                                     \
                break;                                                           \
                                                                                 \
            ___bufSize <<= 1;                                                    \
            ___buf = LLBC_Realloc(char, ___buf, ___bufSize + 1);                 \
        }                                                                        \
        ___buf[___len] = '\0';                                                   \
    } while (0)                                                                  \

#endif // LLBC_TARGET_PLATFORM_WIN32

/**
 * Non-WIN32 platform specified internal macro, use to format string.
 */
#if LLBC_TARGET_PLATFORM_NON_WIN32
#define __LLBC_FormatArg_NonWIN32(fmt, buf, len)                    \
    do {                                                            \
        int &___len = (len);                                        \
        char *&___buf = (buf);                                      \
                                                                    \
        if (UNLIKELY(!(fmt))) {                                     \
            ___len = 0; ___buf = nullptr;                           \
            LLBC_SetLastError(LLBC_ERROR_INVALID);                  \
            break;                                                  \
        }                                                           \
                                                                    \
        va_list ___ap;                                              \
                                                                    \
        int ___bufSize = 1024; ___len = 0;                          \
        ___buf = LLBC_Malloc(char, ___bufSize);                     \
        while (true) {                                              \
            va_start(___ap, (fmt));                                 \
            ___len = ::vsnprintf(___buf, ___bufSize, (fmt), ___ap); \
            va_end(___ap);                                          \
                                                                    \
            /* Workded, break */                                    \
            if (___len > -1 && ___len < ___bufSize)                 \
                break;                                              \
                                                                    \
            /* Try again with more space */                         \
            if (LIKELY(___len > -1)) /* glibc 2.1 and later */      \
                ___bufSize = ___len + 1;                            \
            else /* glibc 2.0 */                                    \
                ___bufSize <<= 1;                                   \
                                                                    \
            ___buf = LLBC_Realloc(char, ___buf, ___bufSize);        \
        }                                                           \
        ___buf[___len] = '\0';                                      \
    } while(0)                                                      \

#endif // LLBC_TARGET_PLATFORM_NON_WIN32

/**
 * RTTI support.
 */
// Define rtti buffer size.
 #define __LLBC_RTTI_BUF_SIZE    512

/**
 * Component generic call method convenience macros define.
 */
// Define comp generic method interface code generate macro.
#define LLBC_COMP_GENERIC_METHOD_INTERFACE(methName)                                     \
    virtual int methName(const LLBC_NS LLBC_Variant &arg, LLBC_NS LLBC_Variant &ret) = 0 \

// Define comp generic method code generate macro.
#define LLBC_COMP_GENERIC_METHOD(methName)                                               \
    virtual int methName(const LLBC_NS LLBC_Variant &arg, LLBC_NS LLBC_Variant &ret)     \

// Define comp generic method implement code macro.
#define LLBC_COMP_GENERIC_METHOD_IMPL(compCls, methName)                                 \
    int compCls::methName(const LLBC_NS LLBC_Variant &arg, LLBC_NS LLBC_Variant &ret)    \

/**
 * Function string format arguments check macro define.
 */
#if LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_STRING_FORMAT_CHECK(fmtIdx, fmtArgsBegIdx)
#else // Non-Win32
 #define LLBC_STRING_FORMAT_CHECK(fmtIdx, fmtArgsBegIdx) __attribute__((format(printf, fmtIdx, fmtArgsBegIdx)))
#endif // LLBC_TARGET_PLATFORM_WIN32

/**
 * The Defer macro impl(Dependency on LLBC_InvokeGuard class).
 */
#define LLBC_Defer(behav) LLBC_NS LLBC_InvokeGuard LLBC_Concat(__invokeGuard__, __LINE__)([&]() { behav; })

#endif // !__LLBC_COM_MACRO_H__
