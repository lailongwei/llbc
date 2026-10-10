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

#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

/**
 * \brief The log config loader.
 *
 * Load log config file into a Config Intermediate Representation (CIR):
 * a plain map/seq/scalar LLBC_Variant tree, carrying no source-format
 * metadata (XML attribute keys, comments, ...). Downstream layers
 * (LoggerConfigurator / LoggerConfigInfo) consume CIR only and stay
 * independent of the underlying config file format.
 *
 * Currently supports XML; kept as a dedicated (log-only) loader so it can
 * be extended with additional formats (yaml/json/...) without touching
 * the general-purpose LLBC config utilities.
 *
 * @note This class is for internal use of the log module only. It is
 *       intentionally not part of the public LLBC config API.
 */
class LLBC_HIDDEN LLBC_LogConfigLoader
{
public:
    /**
     * Load XML config file and convert to CIR.
     *
     * Structural mapping rules (business-agnostic, driven only by XML shape):
     *   - Leaf element (no attributes AND no children) -> Str(inline-text).
     *   - Compound element                             -> Dict:
     *       * every attribute becomes a scalar entry (Str), key = attr name;
     *       * every child recursively converted, keyed by child tag name;
     *       * when a tag name repeats (>=2 siblings under the same parent),
     *         all such siblings are gathered into a Seq; a single occurrence
     *         stays as a bare scalar/Dict.
     *       * on rare attr/child key collision, child wins.
     *   - Document root is treated as a container: its top-level element
     *     (e.g. <Log>) is placed under `cir[<rootTag>]` as a Dict.
     *
     * @param[in]  filePath - the XML config file path.
     * @param[out] cir      - the resulting CIR; on failure will be set to Nil.
     * @return int - LLBC_OK on success, LLBC_FAILED otherwise. On failure a
     *               human-readable detail is recorded via LLBC_SetLastError;
     *               callers can retrieve it with LLBC_FormatLastError().
     */
    static int LoadXmlFromFile(const LLBC_String &filePath,
                               LLBC_Variant &cir);
};

__LLBC_NS_END
