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

//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-FileCopyrightText: 2016-2021 Evan Nemerson <evan@nemerson.com>
// SPDX-License-Identifier: MIT

/* Hedley - https://nemequ.github.io/hedley
 * Created by Evan Nemerson <evan@nemerson.com>
 */

#if !defined(LLBC_JSON_HEDLEY_VERSION) || (LLBC_JSON_HEDLEY_VERSION < 15)
#if defined(LLBC_JSON_HEDLEY_VERSION)
    #undef LLBC_JSON_HEDLEY_VERSION
#endif
#define LLBC_JSON_HEDLEY_VERSION 15

#if defined(LLBC_JSON_HEDLEY_STRINGIFY_EX)
    #undef LLBC_JSON_HEDLEY_STRINGIFY_EX
#endif
#define LLBC_JSON_HEDLEY_STRINGIFY_EX(x) #x

#if defined(LLBC_JSON_HEDLEY_STRINGIFY)
    #undef LLBC_JSON_HEDLEY_STRINGIFY
#endif
#define LLBC_JSON_HEDLEY_STRINGIFY(x) LLBC_JSON_HEDLEY_STRINGIFY_EX(x)

#if defined(LLBC_JSON_HEDLEY_CONCAT_EX)
    #undef LLBC_JSON_HEDLEY_CONCAT_EX
#endif
#define LLBC_JSON_HEDLEY_CONCAT_EX(a,b) a##b

#if defined(LLBC_JSON_HEDLEY_CONCAT)
    #undef LLBC_JSON_HEDLEY_CONCAT
#endif
#define LLBC_JSON_HEDLEY_CONCAT(a,b) LLBC_JSON_HEDLEY_CONCAT_EX(a,b)

#if defined(LLBC_JSON_HEDLEY_CONCAT3_EX)
    #undef LLBC_JSON_HEDLEY_CONCAT3_EX
#endif
#define LLBC_JSON_HEDLEY_CONCAT3_EX(a,b,c) a##b##c

#if defined(LLBC_JSON_HEDLEY_CONCAT3)
    #undef LLBC_JSON_HEDLEY_CONCAT3
#endif
#define LLBC_JSON_HEDLEY_CONCAT3(a,b,c) LLBC_JSON_HEDLEY_CONCAT3_EX(a,b,c)

#if defined(LLBC_JSON_HEDLEY_VERSION_ENCODE)
    #undef LLBC_JSON_HEDLEY_VERSION_ENCODE
#endif
#define LLBC_JSON_HEDLEY_VERSION_ENCODE(major,minor,revision) (((major) * 1000000) + ((minor) * 1000) + (revision))

#if defined(LLBC_JSON_HEDLEY_VERSION_DECODE_MAJOR)
    #undef LLBC_JSON_HEDLEY_VERSION_DECODE_MAJOR
#endif
#define LLBC_JSON_HEDLEY_VERSION_DECODE_MAJOR(version) ((version) / 1000000)

#if defined(LLBC_JSON_HEDLEY_VERSION_DECODE_MINOR)
    #undef LLBC_JSON_HEDLEY_VERSION_DECODE_MINOR
#endif
#define LLBC_JSON_HEDLEY_VERSION_DECODE_MINOR(version) (((version) % 1000000) / 1000)

#if defined(LLBC_JSON_HEDLEY_VERSION_DECODE_REVISION)
    #undef LLBC_JSON_HEDLEY_VERSION_DECODE_REVISION
#endif
#define LLBC_JSON_HEDLEY_VERSION_DECODE_REVISION(version) ((version) % 1000)

#if defined(LLBC_JSON_HEDLEY_GNUC_VERSION)
    #undef LLBC_JSON_HEDLEY_GNUC_VERSION
#endif
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define LLBC_JSON_HEDLEY_GNUC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define LLBC_JSON_HEDLEY_GNUC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, 0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_GNUC_VERSION)
    #define LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_GNUC_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_MSVC_VERSION)
    #undef LLBC_JSON_HEDLEY_MSVC_VERSION
#endif
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define LLBC_JSON_HEDLEY_MSVC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define LLBC_JSON_HEDLEY_MSVC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 10)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define LLBC_JSON_HEDLEY_MSVC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif

#if defined(LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK
#endif
#if !defined(LLBC_JSON_HEDLEY_MSVC_VERSION)
    #define LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (_MSC_FULL_VER >= ((major * 10000000) + (minor * 100000) + (patch)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (_MSC_FULL_VER >= ((major * 1000000) + (minor * 10000) + (patch)))
#else
    #define LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (_MSC_VER >= ((major * 100) + (minor)))
#endif

#if defined(LLBC_JSON_HEDLEY_INTEL_VERSION)
    #undef LLBC_JSON_HEDLEY_INTEL_VERSION
#endif
#if defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE) && !defined(__ICL)
    #define LLBC_JSON_HEDLEY_INTEL_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#elif defined(__INTEL_COMPILER) && !defined(__ICL)
    #define LLBC_JSON_HEDLEY_INTEL_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0)
#endif

#if defined(LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_INTEL_VERSION)
    #define LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_INTEL_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_INTEL_CL_VERSION)
    #undef LLBC_JSON_HEDLEY_INTEL_CL_VERSION
#endif
#if defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE) && defined(__ICL)
    #define LLBC_JSON_HEDLEY_INTEL_CL_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__INTEL_COMPILER, __INTEL_COMPILER_UPDATE, 0)
#endif

#if defined(LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_INTEL_CL_VERSION)
    #define LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_INTEL_CL_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_PGI_VERSION)
    #undef LLBC_JSON_HEDLEY_PGI_VERSION
#endif
#if defined(__PGI) && defined(__PGIC__) && defined(__PGIC_MINOR__) && defined(__PGIC_PATCHLEVEL__)
    #define LLBC_JSON_HEDLEY_PGI_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__)
#endif

#if defined(LLBC_JSON_HEDLEY_PGI_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_PGI_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_PGI_VERSION)
    #define LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_PGI_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_SUNPRO_VERSION)
    #undef LLBC_JSON_HEDLEY_SUNPRO_VERSION
#endif
#if defined(__SUNPRO_C) && (__SUNPRO_C > 0x1000)
    #define LLBC_JSON_HEDLEY_SUNPRO_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE((((__SUNPRO_C >> 16) & 0xf) * 10) + ((__SUNPRO_C >> 12) & 0xf), (((__SUNPRO_C >> 8) & 0xf) * 10) + ((__SUNPRO_C >> 4) & 0xf), (__SUNPRO_C & 0xf) * 10)
#elif defined(__SUNPRO_C)
    #define LLBC_JSON_HEDLEY_SUNPRO_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE((__SUNPRO_C >> 8) & 0xf, (__SUNPRO_C >> 4) & 0xf, (__SUNPRO_C) & 0xf)
#elif defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x1000)
    #define LLBC_JSON_HEDLEY_SUNPRO_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE((((__SUNPRO_CC >> 16) & 0xf) * 10) + ((__SUNPRO_CC >> 12) & 0xf), (((__SUNPRO_CC >> 8) & 0xf) * 10) + ((__SUNPRO_CC >> 4) & 0xf), (__SUNPRO_CC & 0xf) * 10)
#elif defined(__SUNPRO_CC)
    #define LLBC_JSON_HEDLEY_SUNPRO_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE((__SUNPRO_CC >> 8) & 0xf, (__SUNPRO_CC >> 4) & 0xf, (__SUNPRO_CC) & 0xf)
#endif

#if defined(LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_SUNPRO_VERSION)
    #define LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_SUNPRO_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION)
    #undef LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION
#endif
#if defined(__EMSCRIPTEN__)
    #define LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#endif

#if defined(LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION)
    #define LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_ARM_VERSION)
    #undef LLBC_JSON_HEDLEY_ARM_VERSION
#endif
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define LLBC_JSON_HEDLEY_ARM_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define LLBC_JSON_HEDLEY_ARM_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif

#if defined(LLBC_JSON_HEDLEY_ARM_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_ARM_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_ARM_VERSION)
    #define LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_ARM_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_IBM_VERSION)
    #undef LLBC_JSON_HEDLEY_IBM_VERSION
#endif
#if defined(__ibmxl__)
    #define LLBC_JSON_HEDLEY_IBM_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#elif defined(__xlC__) && defined(__xlC_ver__)
    #define LLBC_JSON_HEDLEY_IBM_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__xlC__ >> 8, __xlC__ & 0xff, (__xlC_ver__ >> 8) & 0xff)
#elif defined(__xlC__)
    #define LLBC_JSON_HEDLEY_IBM_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__xlC__ >> 8, __xlC__ & 0xff, 0)
#endif

#if defined(LLBC_JSON_HEDLEY_IBM_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_IBM_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_IBM_VERSION)
    #define LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_IBM_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_VERSION
#endif
#if \
    defined(__TI_COMPILER_VERSION__) && \
    ( \
      defined(__TMS470__) || defined(__TI_ARM__) || \
      defined(__MSP430__) || \
      defined(__TMS320C2000__) \
    )
#if (__TI_COMPILER_VERSION__ >= 16000000)
    #define LLBC_JSON_HEDLEY_TI_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif
#endif

#if defined(LLBC_JSON_HEDLEY_TI_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_VERSION)
    #define LLBC_JSON_HEDLEY_TI_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL2000_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_CL2000_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__TMS320C2000__)
    #define LLBC_JSON_HEDLEY_TI_CL2000_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_CL2000_VERSION)
    #define LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_CL2000_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL430_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_CL430_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__MSP430__)
    #define LLBC_JSON_HEDLEY_TI_CL430_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_CL430_VERSION)
    #define LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_CL430_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_ARMCL_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_ARMCL_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && (defined(__TMS470__) || defined(__TI_ARM__))
    #define LLBC_JSON_HEDLEY_TI_ARMCL_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_ARMCL_VERSION)
    #define LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL6X_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_CL6X_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__TMS320C6X__)
    #define LLBC_JSON_HEDLEY_TI_CL6X_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_CL6X_VERSION)
    #define LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_CL6X_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL7X_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_CL7X_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__C7000__)
    #define LLBC_JSON_HEDLEY_TI_CL7X_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_CL7X_VERSION)
    #define LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_CL7X_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CLPRU_VERSION)
    #undef LLBC_JSON_HEDLEY_TI_CLPRU_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__PRU__)
    #define LLBC_JSON_HEDLEY_TI_CLPRU_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TI_CLPRU_VERSION)
    #define LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TI_CLPRU_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_CRAY_VERSION)
    #undef LLBC_JSON_HEDLEY_CRAY_VERSION
#endif
#if defined(_CRAYC)
    #if defined(_RELEASE_PATCHLEVEL)
        #define LLBC_JSON_HEDLEY_CRAY_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(_RELEASE_MAJOR, _RELEASE_MINOR, _RELEASE_PATCHLEVEL)
    #else
        #define LLBC_JSON_HEDLEY_CRAY_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(_RELEASE_MAJOR, _RELEASE_MINOR, 0)
    #endif
#endif

#if defined(LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_CRAY_VERSION)
    #define LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_CRAY_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_IAR_VERSION)
    #undef LLBC_JSON_HEDLEY_IAR_VERSION
#endif
#if defined(__IAR_SYSTEMS_ICC__)
    #if __VER__ > 1000
        #define LLBC_JSON_HEDLEY_IAR_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE((__VER__ / 1000000), ((__VER__ / 1000) % 1000), (__VER__ % 1000))
    #else
        #define LLBC_JSON_HEDLEY_IAR_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__VER__ / 100, __VER__ % 100, 0)
    #endif
#endif

#if defined(LLBC_JSON_HEDLEY_IAR_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_IAR_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_IAR_VERSION)
    #define LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_IAR_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_TINYC_VERSION)
    #undef LLBC_JSON_HEDLEY_TINYC_VERSION
#endif
#if defined(__TINYC__)
    #define LLBC_JSON_HEDLEY_TINYC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__TINYC__ / 1000, (__TINYC__ / 100) % 10, __TINYC__ % 100)
#endif

#if defined(LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_TINYC_VERSION)
    #define LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_TINYC_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_DMC_VERSION)
    #undef LLBC_JSON_HEDLEY_DMC_VERSION
#endif
#if defined(__DMC__)
    #define LLBC_JSON_HEDLEY_DMC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__DMC__ >> 8, (__DMC__ >> 4) & 0xf, __DMC__ & 0xf)
#endif

#if defined(LLBC_JSON_HEDLEY_DMC_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_DMC_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_DMC_VERSION)
    #define LLBC_JSON_HEDLEY_DMC_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_DMC_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_DMC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_COMPCERT_VERSION)
    #undef LLBC_JSON_HEDLEY_COMPCERT_VERSION
#endif
#if defined(__COMPCERT_VERSION__)
    #define LLBC_JSON_HEDLEY_COMPCERT_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__COMPCERT_VERSION__ / 10000, (__COMPCERT_VERSION__ / 100) % 100, __COMPCERT_VERSION__ % 100)
#endif

#if defined(LLBC_JSON_HEDLEY_COMPCERT_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_COMPCERT_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_COMPCERT_VERSION)
    #define LLBC_JSON_HEDLEY_COMPCERT_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_COMPCERT_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_COMPCERT_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_PELLES_VERSION)
    #undef LLBC_JSON_HEDLEY_PELLES_VERSION
#endif
#if defined(__POCC__)
    #define LLBC_JSON_HEDLEY_PELLES_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__POCC__ / 100, __POCC__ % 100, 0)
#endif

#if defined(LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_PELLES_VERSION)
    #define LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_PELLES_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_MCST_LCC_VERSION)
    #undef LLBC_JSON_HEDLEY_MCST_LCC_VERSION
#endif
#if defined(__LCC__) && defined(__LCC_MINOR__)
    #define LLBC_JSON_HEDLEY_MCST_LCC_VERSION LLBC_JSON_HEDLEY_VERSION_ENCODE(__LCC__ / 100, __LCC__ % 100, __LCC_MINOR__)
#endif

#if defined(LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_MCST_LCC_VERSION)
    #define LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_MCST_LCC_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_VERSION)
    #undef LLBC_JSON_HEDLEY_GCC_VERSION
#endif
#if \
    defined(LLBC_JSON_HEDLEY_GNUC_VERSION) && \
    !defined(__clang__) && \
    !defined(LLBC_JSON_HEDLEY_INTEL_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_PGI_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_ARM_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_CRAY_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_ARMCL_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_CL430_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_CL2000_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_CL6X_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_CL7X_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TI_CLPRU_VERSION) && \
    !defined(__COMPCERT__) && \
    !defined(LLBC_JSON_HEDLEY_MCST_LCC_VERSION)
    #define LLBC_JSON_HEDLEY_GCC_VERSION LLBC_JSON_HEDLEY_GNUC_VERSION
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_GCC_VERSION_CHECK
#endif
#if defined(LLBC_JSON_HEDLEY_GCC_VERSION)
    #define LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch) (LLBC_JSON_HEDLEY_GCC_VERSION >= LLBC_JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_HAS_ATTRIBUTE
#endif
#if \
  defined(__has_attribute) && \
  ( \
    (!defined(LLBC_JSON_HEDLEY_IAR_VERSION) || LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,5,9)) \
  )
#  define LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(attribute) __has_attribute(attribute)
#else
#  define LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(attribute) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_ATTRIBUTE
#endif
#if defined(__has_attribute)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(attribute)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_ATTRIBUTE
#endif
#if defined(__has_attribute)
    #define LLBC_JSON_HEDLEY_GCC_HAS_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(attribute)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE
#endif
#if \
    defined(__has_cpp_attribute) && \
    defined(__cplusplus) && \
    (!defined(LLBC_JSON_HEDLEY_SUNPRO_VERSION) || LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0))
    #define LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(attribute) __has_cpp_attribute(attribute)
#else
    #define LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(attribute) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS)
    #undef LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS
#endif
#if !defined(__cplusplus) || !defined(__has_cpp_attribute)
    #define LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(ns,attribute) (0)
#elif \
    !defined(LLBC_JSON_HEDLEY_PGI_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_IAR_VERSION) && \
    (!defined(LLBC_JSON_HEDLEY_SUNPRO_VERSION) || LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0)) && \
    (!defined(LLBC_JSON_HEDLEY_MSVC_VERSION) || LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(19,20,0))
    #define LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(ns,attribute) LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(ns::attribute)
#else
    #define LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(ns,attribute) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE
#endif
#if defined(__has_cpp_attribute) && defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) __has_cpp_attribute(attribute)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE
#endif
#if defined(__has_cpp_attribute) && defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) __has_cpp_attribute(attribute)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_BUILTIN)
    #undef LLBC_JSON_HEDLEY_HAS_BUILTIN
#endif
#if defined(__has_builtin)
    #define LLBC_JSON_HEDLEY_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
    #define LLBC_JSON_HEDLEY_HAS_BUILTIN(builtin) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_BUILTIN)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_BUILTIN
#endif
#if defined(__has_builtin)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_BUILTIN(builtin,major,minor,patch) __has_builtin(builtin)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_BUILTIN(builtin,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_BUILTIN)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_BUILTIN
#endif
#if defined(__has_builtin)
    #define LLBC_JSON_HEDLEY_GCC_HAS_BUILTIN(builtin,major,minor,patch) __has_builtin(builtin)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_BUILTIN(builtin,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_FEATURE)
    #undef LLBC_JSON_HEDLEY_HAS_FEATURE
#endif
#if defined(__has_feature)
    #define LLBC_JSON_HEDLEY_HAS_FEATURE(feature) __has_feature(feature)
#else
    #define LLBC_JSON_HEDLEY_HAS_FEATURE(feature) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_FEATURE)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_FEATURE
#endif
#if defined(__has_feature)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_FEATURE(feature,major,minor,patch) __has_feature(feature)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_FEATURE(feature,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_FEATURE)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_FEATURE
#endif
#if defined(__has_feature)
    #define LLBC_JSON_HEDLEY_GCC_HAS_FEATURE(feature,major,minor,patch) __has_feature(feature)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_FEATURE(feature,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_EXTENSION)
    #undef LLBC_JSON_HEDLEY_HAS_EXTENSION
#endif
#if defined(__has_extension)
    #define LLBC_JSON_HEDLEY_HAS_EXTENSION(extension) __has_extension(extension)
#else
    #define LLBC_JSON_HEDLEY_HAS_EXTENSION(extension) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_EXTENSION)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_EXTENSION
#endif
#if defined(__has_extension)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_EXTENSION(extension,major,minor,patch) __has_extension(extension)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_EXTENSION(extension,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_EXTENSION)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_EXTENSION
#endif
#if defined(__has_extension)
    #define LLBC_JSON_HEDLEY_GCC_HAS_EXTENSION(extension,major,minor,patch) __has_extension(extension)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_EXTENSION(extension,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE
#endif
#if defined(__has_declspec_attribute)
    #define LLBC_JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE(attribute) __has_declspec_attribute(attribute)
#else
    #define LLBC_JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE(attribute) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE
#endif
#if defined(__has_declspec_attribute)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) __has_declspec_attribute(attribute)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE
#endif
#if defined(__has_declspec_attribute)
    #define LLBC_JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) __has_declspec_attribute(attribute)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_HAS_WARNING)
    #undef LLBC_JSON_HEDLEY_HAS_WARNING
#endif
#if defined(__has_warning)
    #define LLBC_JSON_HEDLEY_HAS_WARNING(warning) __has_warning(warning)
#else
    #define LLBC_JSON_HEDLEY_HAS_WARNING(warning) (0)
#endif

#if defined(LLBC_JSON_HEDLEY_GNUC_HAS_WARNING)
    #undef LLBC_JSON_HEDLEY_GNUC_HAS_WARNING
#endif
#if defined(__has_warning)
    #define LLBC_JSON_HEDLEY_GNUC_HAS_WARNING(warning,major,minor,patch) __has_warning(warning)
#else
    #define LLBC_JSON_HEDLEY_GNUC_HAS_WARNING(warning,major,minor,patch) LLBC_JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_GCC_HAS_WARNING)
    #undef LLBC_JSON_HEDLEY_GCC_HAS_WARNING
#endif
#if defined(__has_warning)
    #define LLBC_JSON_HEDLEY_GCC_HAS_WARNING(warning,major,minor,patch) __has_warning(warning)
#else
    #define LLBC_JSON_HEDLEY_GCC_HAS_WARNING(warning,major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if \
    (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
    defined(__clang__) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0) || \
    LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(18,4,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,7,0) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(2,0,1) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,0,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(5,0,0) || \
    LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,17) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(8,0,0) || \
    (LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) && defined(__C99_PRAGMA_OPERATOR))
    #define LLBC_JSON_HEDLEY_PRAGMA(value) _Pragma(#value)
#elif LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    #define LLBC_JSON_HEDLEY_PRAGMA(value) __pragma(value)
#else
    #define LLBC_JSON_HEDLEY_PRAGMA(value)
#endif

#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH
#endif
#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_POP)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#endif
#if defined(__clang__)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("warning(push)")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("warning(pop)")
#elif LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,6,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH __pragma(warning(push))
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP __pragma(warning(pop))
#elif LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(5,6,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("push")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("pop")
#elif \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,4,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("diag_push")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("diag_pop")
#elif LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(2,90,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("warning(push)")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("warning(pop)")
#else
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#endif

/* LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_ is for
   HEDLEY INTERNAL USE ONLY.  API subject to change without notice. */
#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_
#endif
#if defined(__cplusplus)
#  if LLBC_JSON_HEDLEY_HAS_WARNING("-Wc++98-compat")
#    if LLBC_JSON_HEDLEY_HAS_WARNING("-Wc++17-extensions")
#      if LLBC_JSON_HEDLEY_HAS_WARNING("-Wc++1z-extensions")
#        define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(xpr) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    _Pragma("clang diagnostic ignored \"-Wc++17-extensions\"") \
    _Pragma("clang diagnostic ignored \"-Wc++1z-extensions\"") \
    xpr \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#      else
#        define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(xpr) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    _Pragma("clang diagnostic ignored \"-Wc++17-extensions\"") \
    xpr \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#      endif
#    else
#      define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(xpr) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    xpr \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#    endif
#  endif
#endif
#if !defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(x) x
#endif

#if defined(LLBC_JSON_HEDLEY_CONST_CAST)
    #undef LLBC_JSON_HEDLEY_CONST_CAST
#endif
#if defined(__cplusplus)
#  define LLBC_JSON_HEDLEY_CONST_CAST(T, expr) (const_cast<T>(expr))
#elif \
  LLBC_JSON_HEDLEY_HAS_WARNING("-Wcast-qual") || \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,6,0) || \
  LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
#  define LLBC_JSON_HEDLEY_CONST_CAST(T, expr) (__extension__ ({ \
        LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
        LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL \
        ((T) (expr)); \
        LLBC_JSON_HEDLEY_DIAGNOSTIC_POP \
    }))
#else
#  define LLBC_JSON_HEDLEY_CONST_CAST(T, expr) ((T) (expr))
#endif

#if defined(LLBC_JSON_HEDLEY_REINTERPRET_CAST)
    #undef LLBC_JSON_HEDLEY_REINTERPRET_CAST
#endif
#if defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_REINTERPRET_CAST(T, expr) (reinterpret_cast<T>(expr))
#else
    #define LLBC_JSON_HEDLEY_REINTERPRET_CAST(T, expr) ((T) (expr))
#endif

#if defined(LLBC_JSON_HEDLEY_STATIC_CAST)
    #undef LLBC_JSON_HEDLEY_STATIC_CAST
#endif
#if defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_STATIC_CAST(T, expr) (static_cast<T>(expr))
#else
    #define LLBC_JSON_HEDLEY_STATIC_CAST(T, expr) ((T) (expr))
#endif

#if defined(LLBC_JSON_HEDLEY_CPP_CAST)
    #undef LLBC_JSON_HEDLEY_CPP_CAST
#endif
#if defined(__cplusplus)
#  if LLBC_JSON_HEDLEY_HAS_WARNING("-Wold-style-cast")
#    define LLBC_JSON_HEDLEY_CPP_CAST(T, expr) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wold-style-cast\"") \
    ((T) (expr)) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#  elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,3,0)
#    define LLBC_JSON_HEDLEY_CPP_CAST(T, expr) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("diag_suppress=Pe137") \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#  else
#    define LLBC_JSON_HEDLEY_CPP_CAST(T, expr) ((T) (expr))
#  endif
#else
#  define LLBC_JSON_HEDLEY_CPP_CAST(T, expr) (expr)
#endif

#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wdeprecated-declarations")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#elif LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("warning(disable:1478 1786)")
#elif LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED __pragma(warning(disable:1478 1786))
#elif LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(20,7,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1215,1216,1444,1445")
#elif LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1215,1444")
#elif LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,3,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED __pragma(warning(disable:4996))
#elif LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1215,1444")
#elif \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1291,1718")
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,13,0) && !defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("error_messages(off,E_DEPRECATED_ATT,E_DEPRECATED_ATT_MESS)")
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,13,0) && defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("error_messages(off,symdeprecated,symdeprecated2)")
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress=Pe1444,Pe1215")
#elif LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(2,90,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("warn(disable:2241)")
#else
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED
#endif

#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wunknown-pragmas")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")
#elif LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("warning(disable:161)")
#elif LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS __pragma(warning(disable:161))
#elif LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 1675")
#elif LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,3,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"")
#elif LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS __pragma(warning(disable:4068))
#elif \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(16,9,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,0,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,3,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 163")
#elif LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 163")
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress=Pe161")
#elif LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 161")
#else
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#endif

#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wunknown-attributes")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("clang diagnostic ignored \"-Wunknown-attributes\"")
#elif LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,6,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(17,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("warning(disable:1292)")
#elif LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES __pragma(warning(disable:1292))
#elif LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(19,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES __pragma(warning(disable:5030))
#elif LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(20,7,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1097,1098")
#elif LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1097")
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,14,0) && defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("error_messages(off,attrskipunsup)")
#elif \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(18,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,3,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1173")
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress=Pe1097")
#elif LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1097")
#else
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES
#endif

#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wcast-qual")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma("clang diagnostic ignored \"-Wcast-qual\"")
#elif LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma("warning(disable:2203 2331)")
#elif LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma("GCC diagnostic ignored \"-Wcast-qual\"")
#else
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL
#endif

#if defined(LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION)
    #undef LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wunused-function")
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION _Pragma("clang diagnostic ignored \"-Wunused-function\"")
#elif LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#elif LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(1,0,0)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION __pragma(warning(disable:4505))
#elif LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION _Pragma("diag_suppress 3142")
#else
    #define LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION
#endif

#if defined(LLBC_JSON_HEDLEY_DEPRECATED)
    #undef LLBC_JSON_HEDLEY_DEPRECATED
#endif
#if defined(LLBC_JSON_HEDLEY_DEPRECATED_FOR)
    #undef LLBC_JSON_HEDLEY_DEPRECATED_FOR
#endif
#if \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(14,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_DEPRECATED(since) __declspec(deprecated("Since " # since))
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __declspec(deprecated("Since " #since "; use " #replacement))
#elif \
    (LLBC_JSON_HEDLEY_HAS_EXTENSION(attribute_deprecated_with_message) && !defined(LLBC_JSON_HEDLEY_IAR_VERSION)) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,5,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(5,6,0) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,13,0) || \
    LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(18,1,0) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(18,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,3,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,3,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_DEPRECATED(since) __attribute__((__deprecated__("Since " #since)))
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __attribute__((__deprecated__("Since " #since "; use " #replacement)))
#elif defined(__cplusplus) && (__cplusplus >= 201402L)
    #define LLBC_JSON_HEDLEY_DEPRECATED(since) LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[deprecated("Since " #since)]])
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement) LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[deprecated("Since " #since "; use " #replacement)]])
#elif \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(deprecated) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
    LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
    #define LLBC_JSON_HEDLEY_DEPRECATED(since) __attribute__((__deprecated__))
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __attribute__((__deprecated__))
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(6,50,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_DEPRECATED(since) __declspec(deprecated)
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __declspec(deprecated)
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_DEPRECATED(since) _Pragma("deprecated")
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement) _Pragma("deprecated")
#else
    #define LLBC_JSON_HEDLEY_DEPRECATED(since)
    #define LLBC_JSON_HEDLEY_DEPRECATED_FOR(since, replacement)
#endif

#if defined(LLBC_JSON_HEDLEY_UNAVAILABLE)
    #undef LLBC_JSON_HEDLEY_UNAVAILABLE
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(warning) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,3,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_UNAVAILABLE(available_since) __attribute__((__warning__("Not available until " #available_since)))
#else
    #define LLBC_JSON_HEDLEY_UNAVAILABLE(available_since)
#endif

#if defined(LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT)
    #undef LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT
#endif
#if defined(LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG)
    #undef LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(warn_unused_result) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    (LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0) && defined(__cplusplus)) || \
    LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) __attribute__((__warn_unused_result__))
#elif (LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(nodiscard) >= 201907L)
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard]])
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard(msg)]])
#elif LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(nodiscard)
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard]])
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard]])
#elif defined(_Check_return_) /* SAL */
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT _Check_return_
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) _Check_return_
#else
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT
    #define LLBC_JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg)
#endif

#if defined(LLBC_JSON_HEDLEY_SENTINEL)
    #undef LLBC_JSON_HEDLEY_SENTINEL
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(sentinel) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(5,4,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_SENTINEL(position) __attribute__((__sentinel__(position)))
#else
    #define LLBC_JSON_HEDLEY_SENTINEL(position)
#endif

#if defined(LLBC_JSON_HEDLEY_NO_RETURN)
    #undef LLBC_JSON_HEDLEY_NO_RETURN
#endif
#if LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_NO_RETURN __noreturn
#elif \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_NO_RETURN __attribute__((__noreturn__))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define LLBC_JSON_HEDLEY_NO_RETURN _Noreturn
#elif defined(__cplusplus) && (__cplusplus >= 201103L)
    #define LLBC_JSON_HEDLEY_NO_RETURN LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[noreturn]])
#elif \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(noreturn) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,2,0) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
    #define LLBC_JSON_HEDLEY_NO_RETURN __attribute__((__noreturn__))
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
    #define LLBC_JSON_HEDLEY_NO_RETURN _Pragma("does_not_return")
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_NO_RETURN __declspec(noreturn)
#elif LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,0,0) && defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_NO_RETURN _Pragma("FUNC_NEVER_RETURNS;")
#elif LLBC_JSON_HEDLEY_COMPCERT_VERSION_CHECK(3,2,0)
    #define LLBC_JSON_HEDLEY_NO_RETURN __attribute((noreturn))
#elif LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(9,0,0)
    #define LLBC_JSON_HEDLEY_NO_RETURN __declspec(noreturn)
#else
    #define LLBC_JSON_HEDLEY_NO_RETURN
#endif

#if defined(LLBC_JSON_HEDLEY_NO_ESCAPE)
    #undef LLBC_JSON_HEDLEY_NO_ESCAPE
#endif
#if LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(noescape)
    #define LLBC_JSON_HEDLEY_NO_ESCAPE __attribute__((__noescape__))
#else
    #define LLBC_JSON_HEDLEY_NO_ESCAPE
#endif

#if defined(LLBC_JSON_HEDLEY_UNREACHABLE)
    #undef LLBC_JSON_HEDLEY_UNREACHABLE
#endif
#if defined(LLBC_JSON_HEDLEY_UNREACHABLE_RETURN)
    #undef LLBC_JSON_HEDLEY_UNREACHABLE_RETURN
#endif
#if defined(LLBC_JSON_HEDLEY_ASSUME)
    #undef LLBC_JSON_HEDLEY_ASSUME
#endif
#if \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_ASSUME(expr) __assume(expr)
#elif LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_assume)
    #define LLBC_JSON_HEDLEY_ASSUME(expr) __builtin_assume(expr)
#elif \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(4,0,0)
    #if defined(__cplusplus)
        #define LLBC_JSON_HEDLEY_ASSUME(expr) std::_nassert(expr)
    #else
        #define LLBC_JSON_HEDLEY_ASSUME(expr) _nassert(expr)
    #endif
#endif
#if \
    (LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_unreachable) && (!defined(LLBC_JSON_HEDLEY_ARM_VERSION))) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,5,0) || \
    LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(18,10,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(13,1,5) || \
    LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(10,0,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_UNREACHABLE() __builtin_unreachable()
#elif defined(LLBC_JSON_HEDLEY_ASSUME)
    #define LLBC_JSON_HEDLEY_UNREACHABLE() LLBC_JSON_HEDLEY_ASSUME(0)
#endif
#if !defined(LLBC_JSON_HEDLEY_ASSUME)
    #if defined(LLBC_JSON_HEDLEY_UNREACHABLE)
        #define LLBC_JSON_HEDLEY_ASSUME(expr) LLBC_JSON_HEDLEY_STATIC_CAST(void, ((expr) ? 1 : (LLBC_JSON_HEDLEY_UNREACHABLE(), 1)))
    #else
        #define LLBC_JSON_HEDLEY_ASSUME(expr) LLBC_JSON_HEDLEY_STATIC_CAST(void, expr)
    #endif
#endif
#if defined(LLBC_JSON_HEDLEY_UNREACHABLE)
    #if  \
        LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,0) || \
        LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(4,0,0)
        #define LLBC_JSON_HEDLEY_UNREACHABLE_RETURN(value) return (LLBC_JSON_HEDLEY_STATIC_CAST(void, LLBC_JSON_HEDLEY_ASSUME(0)), (value))
    #else
        #define LLBC_JSON_HEDLEY_UNREACHABLE_RETURN(value) LLBC_JSON_HEDLEY_UNREACHABLE()
    #endif
#else
    #define LLBC_JSON_HEDLEY_UNREACHABLE_RETURN(value) return (value)
#endif
#if !defined(LLBC_JSON_HEDLEY_UNREACHABLE)
    #define LLBC_JSON_HEDLEY_UNREACHABLE() LLBC_JSON_HEDLEY_ASSUME(0)
#endif

LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wpedantic")
    #pragma clang diagnostic ignored "-Wpedantic"
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wc++98-compat-pedantic") && defined(__cplusplus)
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif
#if LLBC_JSON_HEDLEY_GCC_HAS_WARNING("-Wvariadic-macros",4,0,0)
    #if defined(__clang__)
        #pragma clang diagnostic ignored "-Wvariadic-macros"
    #elif defined(LLBC_JSON_HEDLEY_GCC_VERSION)
        #pragma GCC diagnostic ignored "-Wvariadic-macros"
    #endif
#endif
#if defined(LLBC_JSON_HEDLEY_NON_NULL)
    #undef LLBC_JSON_HEDLEY_NON_NULL
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(nonnull) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0)
    #define LLBC_JSON_HEDLEY_NON_NULL(...) __attribute__((__nonnull__(__VA_ARGS__)))
#else
    #define LLBC_JSON_HEDLEY_NON_NULL(...)
#endif
LLBC_JSON_HEDLEY_DIAGNOSTIC_POP

#if defined(LLBC_JSON_HEDLEY_PRINTF_FORMAT)
    #undef LLBC_JSON_HEDLEY_PRINTF_FORMAT
#endif
#if defined(__MINGW32__) && LLBC_JSON_HEDLEY_GCC_HAS_ATTRIBUTE(format,4,4,0) && !defined(__USE_MINGW_ANSI_STDIO)
    #define LLBC_JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __attribute__((__format__(ms_printf, string_idx, first_to_check)))
#elif defined(__MINGW32__) && LLBC_JSON_HEDLEY_GCC_HAS_ATTRIBUTE(format,4,4,0) && defined(__USE_MINGW_ANSI_STDIO)
    #define LLBC_JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __attribute__((__format__(gnu_printf, string_idx, first_to_check)))
#elif \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(format) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(5,6,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __attribute__((__format__(__printf__, string_idx, first_to_check)))
#elif LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(6,0,0)
    #define LLBC_JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __declspec(vaformat(printf,string_idx,first_to_check))
#else
    #define LLBC_JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check)
#endif

#if defined(LLBC_JSON_HEDLEY_CONSTEXPR)
    #undef LLBC_JSON_HEDLEY_CONSTEXPR
#endif
#if defined(__cplusplus)
    #if __cplusplus >= 201103L
        #define LLBC_JSON_HEDLEY_CONSTEXPR LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(constexpr)
    #endif
#endif
#if !defined(LLBC_JSON_HEDLEY_CONSTEXPR)
    #define LLBC_JSON_HEDLEY_CONSTEXPR
#endif

#if defined(LLBC_JSON_HEDLEY_PREDICT)
    #undef LLBC_JSON_HEDLEY_PREDICT
#endif
#if defined(LLBC_JSON_HEDLEY_LIKELY)
    #undef LLBC_JSON_HEDLEY_LIKELY
#endif
#if defined(LLBC_JSON_HEDLEY_UNLIKELY)
    #undef LLBC_JSON_HEDLEY_UNLIKELY
#endif
#if defined(LLBC_JSON_HEDLEY_UNPREDICTABLE)
    #undef LLBC_JSON_HEDLEY_UNPREDICTABLE
#endif
#if LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_unpredictable)
    #define LLBC_JSON_HEDLEY_UNPREDICTABLE(expr) __builtin_unpredictable((expr))
#endif
#if \
  (LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_expect_with_probability) && !defined(LLBC_JSON_HEDLEY_PGI_VERSION)) || \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(9,0,0) || \
  LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#  define LLBC_JSON_HEDLEY_PREDICT(expr, value, probability) __builtin_expect_with_probability(  (expr), (value), (probability))
#  define LLBC_JSON_HEDLEY_PREDICT_TRUE(expr, probability)   __builtin_expect_with_probability(!!(expr),    1   , (probability))
#  define LLBC_JSON_HEDLEY_PREDICT_FALSE(expr, probability)  __builtin_expect_with_probability(!!(expr),    0   , (probability))
#  define LLBC_JSON_HEDLEY_LIKELY(expr)                      __builtin_expect                 (!!(expr),    1                  )
#  define LLBC_JSON_HEDLEY_UNLIKELY(expr)                    __builtin_expect                 (!!(expr),    0                  )
#elif \
  (LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_expect) && !defined(LLBC_JSON_HEDLEY_INTEL_CL_VERSION)) || \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,0,0) || \
  LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  (LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0) && defined(__cplusplus)) || \
  LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,7,0) || \
  LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(3,1,0) || \
  LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,1,0) || \
  LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,1,0) || \
  LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,27) || \
  LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(8,1,0) || \
  LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#  define LLBC_JSON_HEDLEY_PREDICT(expr, expected, probability) \
    (((probability) >= 0.9) ? __builtin_expect((expr), (expected)) : (LLBC_JSON_HEDLEY_STATIC_CAST(void, expected), (expr)))
#  define LLBC_JSON_HEDLEY_PREDICT_TRUE(expr, probability) \
    (__extension__ ({ \
        double hedley_probability_ = (probability); \
        ((hedley_probability_ >= 0.9) ? __builtin_expect(!!(expr), 1) : ((hedley_probability_ <= 0.1) ? __builtin_expect(!!(expr), 0) : !!(expr))); \
    }))
#  define LLBC_JSON_HEDLEY_PREDICT_FALSE(expr, probability) \
    (__extension__ ({ \
        double hedley_probability_ = (probability); \
        ((hedley_probability_ >= 0.9) ? __builtin_expect(!!(expr), 0) : ((hedley_probability_ <= 0.1) ? __builtin_expect(!!(expr), 1) : !!(expr))); \
    }))
#  define LLBC_JSON_HEDLEY_LIKELY(expr)   __builtin_expect(!!(expr), 1)
#  define LLBC_JSON_HEDLEY_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#  define LLBC_JSON_HEDLEY_PREDICT(expr, expected, probability) (LLBC_JSON_HEDLEY_STATIC_CAST(void, expected), (expr))
#  define LLBC_JSON_HEDLEY_PREDICT_TRUE(expr, probability) (!!(expr))
#  define LLBC_JSON_HEDLEY_PREDICT_FALSE(expr, probability) (!!(expr))
#  define LLBC_JSON_HEDLEY_LIKELY(expr) (!!(expr))
#  define LLBC_JSON_HEDLEY_UNLIKELY(expr) (!!(expr))
#endif
#if !defined(LLBC_JSON_HEDLEY_UNPREDICTABLE)
    #define LLBC_JSON_HEDLEY_UNPREDICTABLE(expr) LLBC_JSON_HEDLEY_PREDICT(expr, 1, 0.5)
#endif

#if defined(LLBC_JSON_HEDLEY_MALLOC)
    #undef LLBC_JSON_HEDLEY_MALLOC
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(malloc) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(12,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_MALLOC __attribute__((__malloc__))
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
    #define LLBC_JSON_HEDLEY_MALLOC _Pragma("returns_new_memory")
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(14,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_MALLOC __declspec(restrict)
#else
    #define LLBC_JSON_HEDLEY_MALLOC
#endif

#if defined(LLBC_JSON_HEDLEY_PURE)
    #undef LLBC_JSON_HEDLEY_PURE
#endif
#if \
  LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(pure) || \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(2,96,0) || \
  LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
  LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
  (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
  (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
  (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
  LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
  LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#  define LLBC_JSON_HEDLEY_PURE __attribute__((__pure__))
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
#  define LLBC_JSON_HEDLEY_PURE _Pragma("does_not_write_global_data")
#elif defined(__cplusplus) && \
    ( \
      LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(2,0,1) || \
      LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(4,0,0) || \
      LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) \
    )
#  define LLBC_JSON_HEDLEY_PURE _Pragma("FUNC_IS_PURE;")
#else
#  define LLBC_JSON_HEDLEY_PURE
#endif

#if defined(LLBC_JSON_HEDLEY_CONST)
    #undef LLBC_JSON_HEDLEY_CONST
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(const) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(2,5,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_CONST __attribute__((__const__))
#elif \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
    #define LLBC_JSON_HEDLEY_CONST _Pragma("no_side_effect")
#else
    #define LLBC_JSON_HEDLEY_CONST LLBC_JSON_HEDLEY_PURE
#endif

#if defined(LLBC_JSON_HEDLEY_RESTRICT)
    #undef LLBC_JSON_HEDLEY_RESTRICT
#endif
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && !defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_RESTRICT restrict
#elif \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(14,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,4) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    (LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,14,0) && defined(__cplusplus)) || \
    LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0) || \
    defined(__clang__) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_RESTRICT __restrict
#elif LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,3,0) && !defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_RESTRICT _Restrict
#else
    #define LLBC_JSON_HEDLEY_RESTRICT
#endif

#if defined(LLBC_JSON_HEDLEY_INLINE)
    #undef LLBC_JSON_HEDLEY_INLINE
#endif
#if \
    (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
    (defined(__cplusplus) && (__cplusplus >= 199711L))
    #define LLBC_JSON_HEDLEY_INLINE inline
#elif \
    defined(LLBC_JSON_HEDLEY_GCC_VERSION) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(6,2,0)
    #define LLBC_JSON_HEDLEY_INLINE __inline__
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(12,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(3,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,0,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_INLINE __inline
#else
    #define LLBC_JSON_HEDLEY_INLINE
#endif

#if defined(LLBC_JSON_HEDLEY_ALWAYS_INLINE)
    #undef LLBC_JSON_HEDLEY_ALWAYS_INLINE
#endif
#if \
  LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(always_inline) || \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
  LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
  LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
  (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
  (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
  (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
  LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
  LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
#  define LLBC_JSON_HEDLEY_ALWAYS_INLINE __attribute__((__always_inline__)) LLBC_JSON_HEDLEY_INLINE
#elif \
  LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(12,0,0) || \
  LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
#  define LLBC_JSON_HEDLEY_ALWAYS_INLINE __forceinline
#elif defined(__cplusplus) && \
    ( \
      LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
      LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
      LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
      LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,1,0) || \
      LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
      LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) \
    )
#  define LLBC_JSON_HEDLEY_ALWAYS_INLINE _Pragma("FUNC_ALWAYS_INLINE;")
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
#  define LLBC_JSON_HEDLEY_ALWAYS_INLINE _Pragma("inline=forced")
#else
#  define LLBC_JSON_HEDLEY_ALWAYS_INLINE LLBC_JSON_HEDLEY_INLINE
#endif

#if defined(LLBC_JSON_HEDLEY_NEVER_INLINE)
    #undef LLBC_JSON_HEDLEY_NEVER_INLINE
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(noinline) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    LLBC_JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    LLBC_JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    LLBC_JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
    LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE __attribute__((__noinline__))
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE __declspec(noinline)
#elif LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(10,2,0)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE _Pragma("noinline")
#elif LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,0,0) && defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE _Pragma("FUNC_CANNOT_INLINE;")
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE _Pragma("inline=never")
#elif LLBC_JSON_HEDLEY_COMPCERT_VERSION_CHECK(3,2,0)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE __attribute((noinline))
#elif LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(9,0,0)
    #define LLBC_JSON_HEDLEY_NEVER_INLINE __declspec(noinline)
#else
    #define LLBC_JSON_HEDLEY_NEVER_INLINE
#endif

#if defined(LLBC_JSON_HEDLEY_PRIVATE)
    #undef LLBC_JSON_HEDLEY_PRIVATE
#endif
#if defined(LLBC_JSON_HEDLEY_PUBLIC)
    #undef LLBC_JSON_HEDLEY_PUBLIC
#endif
#if defined(LLBC_JSON_HEDLEY_IMPORT)
    #undef LLBC_JSON_HEDLEY_IMPORT
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
#  define LLBC_JSON_HEDLEY_PRIVATE
#  define LLBC_JSON_HEDLEY_PUBLIC   __declspec(dllexport)
#  define LLBC_JSON_HEDLEY_IMPORT   __declspec(dllimport)
#else
#  if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(visibility) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
    LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(13,1,0) || \
    ( \
      defined(__TI_EABI__) && \
      ( \
        (LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
        LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) \
      ) \
    ) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#    define LLBC_JSON_HEDLEY_PRIVATE __attribute__((__visibility__("hidden")))
#    define LLBC_JSON_HEDLEY_PUBLIC  __attribute__((__visibility__("default")))
#  else
#    define LLBC_JSON_HEDLEY_PRIVATE
#    define LLBC_JSON_HEDLEY_PUBLIC
#  endif
#  define LLBC_JSON_HEDLEY_IMPORT    extern
#endif

#if defined(LLBC_JSON_HEDLEY_NO_THROW)
    #undef LLBC_JSON_HEDLEY_NO_THROW
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(nothrow) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_NO_THROW __attribute__((__nothrow__))
#elif \
    LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(13,1,0) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0)
    #define LLBC_JSON_HEDLEY_NO_THROW __declspec(nothrow)
#else
    #define LLBC_JSON_HEDLEY_NO_THROW
#endif

#if defined(LLBC_JSON_HEDLEY_FALL_THROUGH)
    #undef LLBC_JSON_HEDLEY_FALL_THROUGH
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(fallthrough) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(7,0,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_FALL_THROUGH __attribute__((__fallthrough__))
#elif LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(clang,fallthrough)
    #define LLBC_JSON_HEDLEY_FALL_THROUGH LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[clang::fallthrough]])
#elif LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(fallthrough)
    #define LLBC_JSON_HEDLEY_FALL_THROUGH LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[fallthrough]])
#elif defined(__fallthrough) /* SAL */
    #define LLBC_JSON_HEDLEY_FALL_THROUGH __fallthrough
#else
    #define LLBC_JSON_HEDLEY_FALL_THROUGH
#endif

#if defined(LLBC_JSON_HEDLEY_RETURNS_NON_NULL)
    #undef LLBC_JSON_HEDLEY_RETURNS_NON_NULL
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(returns_nonnull) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,9,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_RETURNS_NON_NULL __attribute__((__returns_nonnull__))
#elif defined(_Ret_notnull_) /* SAL */
    #define LLBC_JSON_HEDLEY_RETURNS_NON_NULL _Ret_notnull_
#else
    #define LLBC_JSON_HEDLEY_RETURNS_NON_NULL
#endif

#if defined(LLBC_JSON_HEDLEY_ARRAY_PARAM)
    #undef LLBC_JSON_HEDLEY_ARRAY_PARAM
#endif
#if \
    defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(__STDC_NO_VLA__) && \
    !defined(__cplusplus) && \
    !defined(LLBC_JSON_HEDLEY_PGI_VERSION) && \
    !defined(LLBC_JSON_HEDLEY_TINYC_VERSION)
    #define LLBC_JSON_HEDLEY_ARRAY_PARAM(name) (name)
#else
    #define LLBC_JSON_HEDLEY_ARRAY_PARAM(name)
#endif

#if defined(LLBC_JSON_HEDLEY_IS_CONSTANT)
    #undef LLBC_JSON_HEDLEY_IS_CONSTANT
#endif
#if defined(LLBC_JSON_HEDLEY_REQUIRE_CONSTEXPR)
    #undef LLBC_JSON_HEDLEY_REQUIRE_CONSTEXPR
#endif
/* LLBC_JSON_HEDLEY_IS_CONSTEXPR_ is for
   HEDLEY INTERNAL USE ONLY.  API subject to change without notice. */
#if defined(LLBC_JSON_HEDLEY_IS_CONSTEXPR_)
    #undef LLBC_JSON_HEDLEY_IS_CONSTEXPR_
#endif
#if \
    LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_constant_p) || \
    LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0) || \
    LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,19) || \
    LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(13,1,0) || \
    LLBC_JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,1,0) || \
    (LLBC_JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0) && !defined(__cplusplus)) || \
    LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(8,1,0) || \
    LLBC_JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define LLBC_JSON_HEDLEY_IS_CONSTANT(expr) __builtin_constant_p(expr)
#endif
#if !defined(__cplusplus)
#  if \
       LLBC_JSON_HEDLEY_HAS_BUILTIN(__builtin_types_compatible_p) || \
       LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0) || \
       LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
       LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(13,1,0) || \
       LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(8,1,0) || \
       LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(5,4,0) || \
       LLBC_JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,24)
#if defined(__INTPTR_TYPE__)
    #define LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr) __builtin_types_compatible_p(__typeof__((1 ? (void*) ((__INTPTR_TYPE__) ((expr) * 0)) : (int*) 0)), int*)
#else
    #include <stdint.h>
    #define LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr) __builtin_types_compatible_p(__typeof__((1 ? (void*) ((intptr_t) ((expr) * 0)) : (int*) 0)), int*)
#endif
#  elif \
       ( \
          defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && \
          !defined(LLBC_JSON_HEDLEY_SUNPRO_VERSION) && \
          !defined(LLBC_JSON_HEDLEY_PGI_VERSION) && \
          !defined(LLBC_JSON_HEDLEY_IAR_VERSION)) || \
       (LLBC_JSON_HEDLEY_HAS_EXTENSION(c_generic_selections) && !defined(LLBC_JSON_HEDLEY_IAR_VERSION)) || \
       LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,9,0) || \
       LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(17,0,0) || \
       LLBC_JSON_HEDLEY_IBM_VERSION_CHECK(12,1,0) || \
       LLBC_JSON_HEDLEY_ARM_VERSION_CHECK(5,3,0)
#if defined(__INTPTR_TYPE__)
    #define LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr) _Generic((1 ? (void*) ((__INTPTR_TYPE__) ((expr) * 0)) : (int*) 0), int*: 1, void*: 0)
#else
    #include <stdint.h>
    #define LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr) _Generic((1 ? (void*) ((intptr_t) * 0) : (int*) 0), int*: 1, void*: 0)
#endif
#  elif \
       defined(LLBC_JSON_HEDLEY_GCC_VERSION) || \
       defined(LLBC_JSON_HEDLEY_INTEL_VERSION) || \
       defined(LLBC_JSON_HEDLEY_TINYC_VERSION) || \
       defined(LLBC_JSON_HEDLEY_TI_ARMCL_VERSION) || \
       LLBC_JSON_HEDLEY_TI_CL430_VERSION_CHECK(18,12,0) || \
       defined(LLBC_JSON_HEDLEY_TI_CL2000_VERSION) || \
       defined(LLBC_JSON_HEDLEY_TI_CL6X_VERSION) || \
       defined(LLBC_JSON_HEDLEY_TI_CL7X_VERSION) || \
       defined(LLBC_JSON_HEDLEY_TI_CLPRU_VERSION) || \
       defined(__clang__)
#    define LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr) ( \
        sizeof(void) != \
        sizeof(*( \
                  1 ? \
                  ((void*) ((expr) * 0L) ) : \
((struct { char v[sizeof(void) * 2]; } *) 1) \
                ) \
              ) \
                                            )
#  endif
#endif
#if defined(LLBC_JSON_HEDLEY_IS_CONSTEXPR_)
    #if !defined(LLBC_JSON_HEDLEY_IS_CONSTANT)
        #define LLBC_JSON_HEDLEY_IS_CONSTANT(expr) LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr)
    #endif
    #define LLBC_JSON_HEDLEY_REQUIRE_CONSTEXPR(expr) (LLBC_JSON_HEDLEY_IS_CONSTEXPR_(expr) ? (expr) : (-1))
#else
    #if !defined(LLBC_JSON_HEDLEY_IS_CONSTANT)
        #define LLBC_JSON_HEDLEY_IS_CONSTANT(expr) (0)
    #endif
    #define LLBC_JSON_HEDLEY_REQUIRE_CONSTEXPR(expr) (expr)
#endif

#if defined(LLBC_JSON_HEDLEY_BEGIN_C_DECLS)
    #undef LLBC_JSON_HEDLEY_BEGIN_C_DECLS
#endif
#if defined(LLBC_JSON_HEDLEY_END_C_DECLS)
    #undef LLBC_JSON_HEDLEY_END_C_DECLS
#endif
#if defined(LLBC_JSON_HEDLEY_C_DECL)
    #undef LLBC_JSON_HEDLEY_C_DECL
#endif
#if defined(__cplusplus)
    #define LLBC_JSON_HEDLEY_BEGIN_C_DECLS extern "C" {
    #define LLBC_JSON_HEDLEY_END_C_DECLS }
    #define LLBC_JSON_HEDLEY_C_DECL extern "C"
#else
    #define LLBC_JSON_HEDLEY_BEGIN_C_DECLS
    #define LLBC_JSON_HEDLEY_END_C_DECLS
    #define LLBC_JSON_HEDLEY_C_DECL
#endif

#if defined(LLBC_JSON_HEDLEY_STATIC_ASSERT)
    #undef LLBC_JSON_HEDLEY_STATIC_ASSERT
#endif
#if \
  !defined(__cplusplus) && ( \
      (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) || \
      (LLBC_JSON_HEDLEY_HAS_FEATURE(c_static_assert) && !defined(LLBC_JSON_HEDLEY_INTEL_CL_VERSION)) || \
      LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(6,0,0) || \
      LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
      defined(_Static_assert) \
    )
#  define LLBC_JSON_HEDLEY_STATIC_ASSERT(expr, message) _Static_assert(expr, message)
#elif \
  (defined(__cplusplus) && (__cplusplus >= 201103L)) || \
  LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(16,0,0) || \
  LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
#  define LLBC_JSON_HEDLEY_STATIC_ASSERT(expr, message) LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(static_assert(expr, message))
#else
#  define LLBC_JSON_HEDLEY_STATIC_ASSERT(expr, message)
#endif

#if defined(LLBC_JSON_HEDLEY_NULL)
    #undef LLBC_JSON_HEDLEY_NULL
#endif
#if defined(__cplusplus)
    #if __cplusplus >= 201103L
        #define LLBC_JSON_HEDLEY_NULL LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(nullptr)
    #elif defined(NULL)
        #define LLBC_JSON_HEDLEY_NULL NULL
    #else
        #define LLBC_JSON_HEDLEY_NULL LLBC_JSON_HEDLEY_STATIC_CAST(void*, 0)
    #endif
#elif defined(NULL)
    #define LLBC_JSON_HEDLEY_NULL NULL
#else
    #define LLBC_JSON_HEDLEY_NULL ((void*) 0)
#endif

#if defined(LLBC_JSON_HEDLEY_MESSAGE)
    #undef LLBC_JSON_HEDLEY_MESSAGE
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wunknown-pragmas")
#  define LLBC_JSON_HEDLEY_MESSAGE(msg) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS \
    LLBC_JSON_HEDLEY_PRAGMA(message msg) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#elif \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,4,0) || \
  LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
#  define LLBC_JSON_HEDLEY_MESSAGE(msg) LLBC_JSON_HEDLEY_PRAGMA(message msg)
#elif LLBC_JSON_HEDLEY_CRAY_VERSION_CHECK(5,0,0)
#  define LLBC_JSON_HEDLEY_MESSAGE(msg) LLBC_JSON_HEDLEY_PRAGMA(_CRI message msg)
#elif LLBC_JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
#  define LLBC_JSON_HEDLEY_MESSAGE(msg) LLBC_JSON_HEDLEY_PRAGMA(message(msg))
#elif LLBC_JSON_HEDLEY_PELLES_VERSION_CHECK(2,0,0)
#  define LLBC_JSON_HEDLEY_MESSAGE(msg) LLBC_JSON_HEDLEY_PRAGMA(message(msg))
#else
#  define LLBC_JSON_HEDLEY_MESSAGE(msg)
#endif

#if defined(LLBC_JSON_HEDLEY_WARNING)
    #undef LLBC_JSON_HEDLEY_WARNING
#endif
#if LLBC_JSON_HEDLEY_HAS_WARNING("-Wunknown-pragmas")
#  define LLBC_JSON_HEDLEY_WARNING(msg) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS \
    LLBC_JSON_HEDLEY_PRAGMA(clang warning msg) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#elif \
  LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(4,8,0) || \
  LLBC_JSON_HEDLEY_PGI_VERSION_CHECK(18,4,0) || \
  LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
#  define LLBC_JSON_HEDLEY_WARNING(msg) LLBC_JSON_HEDLEY_PRAGMA(GCC warning msg)
#elif \
  LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0) || \
  LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
#  define LLBC_JSON_HEDLEY_WARNING(msg) LLBC_JSON_HEDLEY_PRAGMA(message(msg))
#else
#  define LLBC_JSON_HEDLEY_WARNING(msg) LLBC_JSON_HEDLEY_MESSAGE(msg)
#endif

#if defined(LLBC_JSON_HEDLEY_REQUIRE)
    #undef LLBC_JSON_HEDLEY_REQUIRE
#endif
#if defined(LLBC_JSON_HEDLEY_REQUIRE_MSG)
    #undef LLBC_JSON_HEDLEY_REQUIRE_MSG
#endif
#if LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(diagnose_if)
#  if LLBC_JSON_HEDLEY_HAS_WARNING("-Wgcc-compat")
#    define LLBC_JSON_HEDLEY_REQUIRE(expr) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wgcc-compat\"") \
    __attribute__((diagnose_if(!(expr), #expr, "error"))) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#    define LLBC_JSON_HEDLEY_REQUIRE_MSG(expr,msg) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wgcc-compat\"") \
    __attribute__((diagnose_if(!(expr), msg, "error"))) \
    LLBC_JSON_HEDLEY_DIAGNOSTIC_POP
#  else
#    define LLBC_JSON_HEDLEY_REQUIRE(expr) __attribute__((diagnose_if(!(expr), #expr, "error")))
#    define LLBC_JSON_HEDLEY_REQUIRE_MSG(expr,msg) __attribute__((diagnose_if(!(expr), msg, "error")))
#  endif
#else
#  define LLBC_JSON_HEDLEY_REQUIRE(expr)
#  define LLBC_JSON_HEDLEY_REQUIRE_MSG(expr,msg)
#endif

#if defined(LLBC_JSON_HEDLEY_FLAGS)
    #undef LLBC_JSON_HEDLEY_FLAGS
#endif
#if LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(flag_enum) && (!defined(__cplusplus) || LLBC_JSON_HEDLEY_HAS_WARNING("-Wbitfield-enum-conversion"))
    #define LLBC_JSON_HEDLEY_FLAGS __attribute__((__flag_enum__))
#else
    #define LLBC_JSON_HEDLEY_FLAGS
#endif

#if defined(LLBC_JSON_HEDLEY_FLAGS_CAST)
    #undef LLBC_JSON_HEDLEY_FLAGS_CAST
#endif
#if LLBC_JSON_HEDLEY_INTEL_VERSION_CHECK(19,0,0)
#  define LLBC_JSON_HEDLEY_FLAGS_CAST(T, expr) (__extension__ ({ \
        LLBC_JSON_HEDLEY_DIAGNOSTIC_PUSH \
        _Pragma("warning(disable:188)") \
        ((T) (expr)); \
        LLBC_JSON_HEDLEY_DIAGNOSTIC_POP \
    }))
#else
#  define LLBC_JSON_HEDLEY_FLAGS_CAST(T, expr) LLBC_JSON_HEDLEY_STATIC_CAST(T, expr)
#endif

#if defined(LLBC_JSON_HEDLEY_EMPTY_BASES)
    #undef LLBC_JSON_HEDLEY_EMPTY_BASES
#endif
#if \
    (LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(19,0,23918) && !LLBC_JSON_HEDLEY_MSVC_VERSION_CHECK(20,0,0)) || \
    LLBC_JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define LLBC_JSON_HEDLEY_EMPTY_BASES __declspec(empty_bases)
#else
    #define LLBC_JSON_HEDLEY_EMPTY_BASES
#endif

/* Remaining macros are deprecated. */

#if defined(LLBC_JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK)
    #undef LLBC_JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK
#endif
#if defined(__clang__)
    #define LLBC_JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK(major,minor,patch) (0)
#else
    #define LLBC_JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK(major,minor,patch) LLBC_JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_ATTRIBUTE
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_ATTRIBUTE(attribute) LLBC_JSON_HEDLEY_HAS_ATTRIBUTE(attribute)

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_CPP_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_CPP_ATTRIBUTE
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_CPP_ATTRIBUTE(attribute) LLBC_JSON_HEDLEY_HAS_CPP_ATTRIBUTE(attribute)

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_BUILTIN)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_BUILTIN
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_BUILTIN(builtin) LLBC_JSON_HEDLEY_HAS_BUILTIN(builtin)

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_FEATURE)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_FEATURE
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_FEATURE(feature) LLBC_JSON_HEDLEY_HAS_FEATURE(feature)

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_EXTENSION)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_EXTENSION
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_EXTENSION(extension) LLBC_JSON_HEDLEY_HAS_EXTENSION(extension)

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_DECLSPEC_DECLSPEC_ATTRIBUTE)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_DECLSPEC_DECLSPEC_ATTRIBUTE
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_DECLSPEC_ATTRIBUTE(attribute) LLBC_JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE(attribute)

#if defined(LLBC_JSON_HEDLEY_CLANG_HAS_WARNING)
    #undef LLBC_JSON_HEDLEY_CLANG_HAS_WARNING
#endif
#define LLBC_JSON_HEDLEY_CLANG_HAS_WARNING(warning) LLBC_JSON_HEDLEY_HAS_WARNING(warning)

#endif /* !defined(LLBC_JSON_HEDLEY_VERSION) || (LLBC_JSON_HEDLEY_VERSION < X) */
