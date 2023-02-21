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

__LLBC_NS_BEGIN

inline LLBC_ObjectPoolOrderedDeleteNode::LLBC_ObjectPoolOrderedDeleteNode(const LLBC_CString &name)
: _name(name)
, _frontNode(nullptr)
, _backNodes(nullptr)
{
}

inline LLBC_ObjectPoolOrderedDeleteNode::~LLBC_ObjectPoolOrderedDeleteNode()
{
    if (_backNodes)
        delete _backNodes;
}

inline const LLBC_CString &LLBC_ObjectPoolOrderedDeleteNode::GetNodeName() const
{
    return _name;
}

inline LLBC_ObjectPoolOrderedDeleteNode *LLBC_ObjectPoolOrderedDeleteNode::GetFrontNode()
{
    return _frontNode;
}

inline LLBC_ObjectPoolOrderedDeleteNode *LLBC_ObjectPoolOrderedDeleteNode::GetTopFrontNode()
{
    if (!_frontNode)
        return nullptr;
    else if (!_frontNode->GetFrontNode())
        return _frontNode;
    else
        return _frontNode->GetTopFrontNode();
}

inline const LLBC_ObjectPoolOrderedDeleteNodes *LLBC_ObjectPoolOrderedDeleteNode::GetBackNodes() const
{
    return _backNodes;
}

inline int LLBC_ObjectPoolOrderedDeleteNode::AddBackNode(LLBC_ObjectPoolOrderedDeleteNode *backNode)
{
    // Repeat check.
    if (*backNode == *this || IsBackNode(backNode->GetNodeName()))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Set back node _frontNode member.
    backNode->_frontNode = this;
    // Insert to _backNodes container.
    if (!_backNodes)
        _backNodes = new LLBC_ObjectPoolOrderedDeleteNodes;
    _backNodes->insert(std::make_pair(backNode->GetNodeName(), backNode));

    return LLBC_OK;
}

inline int LLBC_ObjectPoolOrderedDeleteNode::RemoveBackNode(const LLBC_CString &name, bool del, bool force)
{
    if (UNLIKELY(_name == name))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (!_backNodes)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // Found and remove back node.
    for (LLBC_ObjectPoolOrderedDeleteNodes::iterator backNodeIt = _backNodes->begin();
         backNodeIt != _backNodes->end();
         ++backNodeIt)
    {
        // Recursive found and remove.
        LLBC_ObjectPoolOrderedDeleteNode *&backNode = backNodeIt->second;
        if (backNode->GetNodeName() != name)
        {
            if (backNode->RemoveBackNode(name, del) == LLBC_OK)
                return LLBC_OK;

            continue;
        }

        // Current node's back node is will remove node, remove it.
        if (!force)
            backNode->AdjustBackNodesFrontNode(this);
        else
            backNode->_frontNode = nullptr;

        if (del)
            delete backNode;
        _backNodes->erase(backNodeIt);

        return LLBC_OK;
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return LLBC_FAILED;
}

inline int LLBC_ObjectPoolOrderedDeleteNode::AdjustBackNodesFrontNode(LLBC_ObjectPoolOrderedDeleteNode *frontNode)
{
    // If front node not specified, use this node's front node.
    if (!frontNode)
        frontNode = _frontNode;

    // frontNode parameter check.
    if (UNLIKELY(!frontNode))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    // If not found any back nodes, return OK.
    if (!_backNodes)
        return LLBC_OK;

    if (!frontNode->_backNodes)
        frontNode->_backNodes = new LLBC_ObjectPoolOrderedDeleteNodes;

    // Foreach adjust all back nodes.
    for (LLBC_ObjectPoolOrderedDeleteNodes::iterator it = _backNodes->begin();
         it != _backNodes->end();
         ++it)
    {
        LLBC_ObjectPoolOrderedDeleteNode *&node = it->second;
        node->_frontNode = frontNode;
        frontNode->_backNodes->insert(std::make_pair(node->GetNodeName(), node));
    }

    // Clear current node's _backNodes.
    _backNodes->clear();
    delete _backNodes;
    _backNodes = nullptr;

    return LLBC_OK;
}

inline bool LLBC_ObjectPoolOrderedDeleteNode::IsFrontNode(const LLBC_CString &name) const
{
    if (!_frontNode)
        return false;

    return (_frontNode->_name == name || _frontNode->IsFrontNode(name));
}

inline bool LLBC_ObjectPoolOrderedDeleteNode::IsBackNode(const LLBC_CString &name) const
{
    if (!_backNodes)
        return false;

    for (LLBC_ObjectPoolOrderedDeleteNodes::const_iterator it = _backNodes->begin();
         it != _backNodes->end();
         ++it)
    {
        if (it->first == name ||
            it->second->IsBackNode(name))
            return true;
    }

    return false;
}

inline bool LLBC_ObjectPoolOrderedDeleteNode::operator ==(const LLBC_ObjectPoolOrderedDeleteNode &other) const
{
    return _name == other._name;
}

__LLBC_NS_END
