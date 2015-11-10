/**
 * @file    PFConfig.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/16
 * @version 1.0
 *
 * @brief   llbc library target platform determine header file.
 */
#ifndef __LLBC_COM_PF_CONFIG_H__
#define __LLBC_COM_PF_CONFIG_H__

// Platform type enumeration.
#define LLBC_PLATFORM_UNKNOWN       0
#define LLBC_PLATFORM_LINUX         1
#define LLBC_PLATFORM_WIN32         2
#define LLBC_PLATFORM_IPHONE        3
#define LLBC_PLATFORM_MAC           4
#define LLBC_PLATFORM_ANDROID       5

// Determine target platform by compile enviornment macro.
#define LLBC_TARGET_PLATFORM        LLBC_PLATFORM_UNKNOWN

// LINUX Platform determine.
#if !LLBC_TARGET_PLATFORM && defined(LINUX)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_LINUX
#endif

// WIN32 Platform determine.
#if !LLBC_TARGET_PLATFORM && defined(WIN32)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_WIN32
#endif

// IPHONE(and IPHONE Simulator)
#if !LLBC_TARGET_PLATFORM && (defined(TARGET_OS_IPHONE) || defined(TARGET_OS_SIMULATOR))
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_IPHONE
#endif
	
// MAC
#if !LLBC_TARGET_PLATFORM && defined(TARGET_OS_MAC)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_MAC
#endif

// ANDROID
#if !LLBC_TARGET_PLATFORM && defined(ANDROID)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_ANDROID
#endif

// Lastest, check LLBC_TARGET_PLATFORM macro set or not.
#if !LLBC_TARGET_PLATFORM
 #error "Cannot recognize the target platform; are you targeting an unsuported platform?"
#endif

// Linux/Non-Linux platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_LINUX
 #define LLBC_TARGET_PLATFORM_LINUX                         1
 #define LLBC_TARGET_PLATFORM_NON_LINUX                     0
#else
 #define LLBC_TARGET_PLATFORM_LINUX                         0
 #define LLBC_TARGET_PLATFORM_NON_LINUX                     1
#endif

// WIN32/Non-WIN32 platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_WIN32
 #define LLBC_TARGET_PLATFORM_WIN32                         1
 #define LLBC_TARGET_PLATFORM_NON_WIN32                     0
#else
 #define LLBC_TARGET_PLATFORM_WIN32                         0
 #define LLBC_TARGET_PLATFORM_NON_WIN32                     1
#endif

// Iphone/Non-Iphone platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_IPHONE
 #define LLBC_TARGET_PLATFORM_IPHONE                        1
 #define LLBC_TARGET_PLATFORM_NON_IPHONE                    0
#else
 #define LLBC_TARGET_PLATFORM_IPHONE                        0
 #define LLBC_TARGET_PLATFORM_NON_IPHONE                    1
#endif

// Mac/Non-Mac platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_MAC
 #define LLBC_TARGET_PLATFORM_MAC                           1
 #define LLBC_TARGET_PLATFORM_NON_MAC                       0
#else
 #define LLBC_TARGET_PLATFORM_MAC                           0
 #define LLBC_TARGET_PLATFORM_NON_MAC                       1
#endif

// Android/Non-Android platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_ANDROID
 #define LLBC_TARGET_PLATFORM_ANDROID                       1
 #define LLBC_TARGET_PLATFORM_NON_ANDROID                   0
#else
 #define LLBC_TARGET_PLATFORM_ANDROID                       0
 #define LLBC_TARGET_PLATFORM_NON_ANDROID                   1
#endif

#endif // !__LLBC_COM_PF_CONFIG_H__

