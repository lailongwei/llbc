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

/*
 * Pre-declare some classes.
 */
class LLBC_ObjectPoolOrderedDeleteNode;
typedef std::map<LLBC_CString, LLBC_ObjectPoolOrderedDeleteNode *> LLBC_ObjectPoolOrderedDeleteNodes;

/**
 * \brief The object pool ordered delete node(object pool instance) encapsulation.
 */
class LLBC_EXPORT LLBC_ObjectPoolOrderedDeleteNode
{
public:
    explicit LLBC_ObjectPoolOrderedDeleteNode(const LLBC_CString &name);
    ~LLBC_ObjectPoolOrderedDeleteNode();

public:
    /**
     * Get the node name(pool instance name).
     * @return const LLBC_CString & - the ordered node name.
     */
    const LLBC_CString &GetNodeName() const;

    /**
     * Get front node.
     * @return LLBC_ObjectPoolOrderedDeleteNode * - the front node.
     */
    LLBC_ObjectPoolOrderedDeleteNode *GetFrontNode();

    /**
     * Get top front node.
     * @return LLBC_ObjectPoolOrderedDeleteNode * - the top front node.
     */
    LLBC_ObjectPoolOrderedDeleteNode *GetTopFrontNode();

    /**
     * Get back ordered nodes set.
     * @return const LLBC_ObjectPoolOrderedDeleteNodes * - the back ordered nodes collection(may be null).
     */
    const LLBC_ObjectPoolOrderedDeleteNodes *GetBackNodes() const;

public:
    /**
     * Add back node.
     * @param[in] backNode - the back node.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddBackNode(LLBC_ObjectPoolOrderedDeleteNode *backNode);

    /**
     * Remove back node.
     * @param[in] name  - the back node name.
     * @param[in] del   - delete back lock flag.
     * @param[in] force - force remove flag.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveBackNode(const LLBC_CString &name, bool del, bool force = false);

    /**
     * Adjust back nodes front node.
     * @param[in] frontNode - the front node, if not specific, will use this node's front node.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AdjustBackNodesFrontNode(LLBC_ObjectPoolOrderedDeleteNode *frontNode = nullptr);

    /**
     * Recursive check given node name is front node or node.
     * @name[in] name - the node name(pool instance name).
     * @return bool - return true if is front node. otherwise return false.
     */
    bool IsFrontNode(const LLBC_CString &name) const;

    /**
     * Recursive check given node name is back node or not.
     * @param[in] name - the node name(pool instance name).
     * @return bool - return true if is back node, otherwise return false.
     */
    bool IsBackNode(const LLBC_CString &name) const;

public:
    /**
     * Operator == implement.
     */
    bool operator ==(const LLBC_ObjectPoolOrderedDeleteNode &other) const;

private:
    LLBC_CString _name;
    LLBC_ObjectPoolOrderedDeleteNode *_frontNode;
    LLBC_ObjectPoolOrderedDeleteNodes *_backNodes;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolOrderedDeleteNodeInl.h"


