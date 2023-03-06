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
 * \brief The bundle class encapsulation.
 */
class LLBC_EXPORT LLBC_Bundle
{
public:
    LLBC_Bundle();
    virtual ~LLBC_Bundle();

public:
    /**
     * Create main bundle(When llbc lib startup, will autocall this method).
     * @return int - return 0 if success, otherwise return -1.
     */
    static int CreateMainBundle();

    /**
     * Destroy main bundle(When llbc lib cleanup, will autocall this method).
     */
    static void DestroyMainBundle();

    /**
     * Get main bundle.
     * @return const LLBC_Bundle * - main bundle object.
     */
    static const LLBC_Bundle *GetMainBundle();

public:
    /**
     * Initialize Bundle object.
     * @param[in] path - bundle path.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &path);

    /**
     * Finalize method.
     */
    void Finalize();

public:
    /**
     * Get bundle handle.
     * Warn: 
     *      Unsafe method, if you need to use this method, please see about NSBundle reference manual.
     * @return LLBC_BundleHandle - bundle handle.
     */
    LLBC_BundleHandle GetHandle() const;

    /**
     * Get bundle name.
     * @return const LLBC_String & - bundle name.
     */
    const LLBC_String &GetBundleName() const;

    /**
     * Get bundle path.
     * @return const LLBC_Bundle & - bundle path.
     */
    const LLBC_String &GetBundlePath() const;

public:
    /**
     * Get specific resource path.
     * @param[in] name - resource name.
     * @return LLBC_String - resource name, if .empty() is true, call LLBC_GetLastError() to get error reason.
     */
    LLBC_String GetResPath(const LLBC_String &name) const;

    /**
     * Get specific resource path.
     * @param[in] name - resource name.
     * @param[in] ext  - resource extension name.
     * @return LLBC_String - resource name, if .empty() is true, call LLBC_GetLastError() to get error reason.
     */
    LLBC_String GetResPath(const LLBC_String &name, const LLBC_String &ext) const;
    
    /**
     * Get specific resource path.
     * @param[in] name  - resource name.
     * @param[in] ext   - resource extension name.
     * @param[in] inDir - directory.
     * @return LLBC_String - resource name, if .empty() is true, call LLBC_GetLastError() to get error reason.
     */
    LLBC_String GetResPath(const LLBC_String &name, const LLBC_String &ext, const LLBC_String &inDir) const;
    
private:
    LLBC_BundleHandle _bundle;

    LLBC_String _bundleName;
    LLBC_String _bundlePath;
};

__LLBC_NS_END
