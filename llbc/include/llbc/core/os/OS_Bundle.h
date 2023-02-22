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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Get main bundle path.
 * @return LLBC_String - main bundle path.
 */
LLBC_EXPORT LLBC_String LLBC_GetMainBundlePath();

/**
 * Create Bundle.
 * @param[in] path - bundle path(if path is empty(.empty() is true), will return main bundle handle.
 * @return LLBC_BundleHandle - bundle handle.
 */
LLBC_EXPORT LLBC_BundleHandle LLBC_CreateBundle(const LLBC_String &path);

/**
 * Release bundle handle.
 * @param[in] bundle - bundle handle.
 */
LLBC_EXPORT void LLBC_ReleaseBundle(LLBC_BundleHandle bundle);

/**
 * Get bundle path.
 * @param[in] bundle - bundle handle.
 * @return LLBC_String - bundle path.
 */
LLBC_EXPORT LLBC_String LLBC_GetBundlePath(LLBC_BundleHandle bundle);

/**
 * Get resource path by name.
 * @param[in] bundle - bundle handle(if set to LLBC_INVALID_BUNDLE_HANDLE, will use main bundle to search).
 * @param[in] name   - resource name.
 * @return LLBC_String - resource path.
 */
LLBC_EXPORT LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle,
                                              const LLBC_String &name);

/**
 * Get resource path by name and extension name.
 * @param[in] bundle - bundle handle(if set to LLBC_INVALID_BUNDLE_HANDLE, will use main bundle to search).
 * @param[in] name   - resource name.
 * @param[in] ext   - resource extension.
 * @return LLBC_String - resource path.
 */
LLBC_EXPORT LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle,
                                              const LLBC_String &name,
                                              const LLBC_String &ext);

/**
 * Get resource path by name and extension name and relative path.
 * @param[in] bundle - bundle handle(if set to LLBC_INVALID_BUNDLE_HANDLE, will use main bundle to search).
 * @param[in] name   - resource name.
 * @param[in] ext    - resource extension.
 * @param[in] inDir  - relative directory path.
 * @return LLBC_String - resource path.
 */
LLBC_EXPORT LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle,
                                              const LLBC_String &name,
                                              const LLBC_String &ext,
                                              const LLBC_String &inDir);


__LLBC_NS_END
