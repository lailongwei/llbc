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

#include "llbc/comm/protocol/IProtocol.h"

__LLBC_NS_BEGIN

/**
 * \brief The Compress-Layer protocol implement.
 */
class LLBC_EXPORT LLBC_CompressProtocol : public LLBC_IProtocol
{
public:
    LLBC_CompressProtocol();
    virtual ~LLBC_CompressProtocol();

public:
    /**
     * Get the protocol layer.
     * @return int - the protocol layer.
     */
    virtual int GetLayer() const;

public:
    /**
     * When data send, will call this method.
     * @param[in] in             - the in data.
     * @param[out] out           - the out data.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(void *in, void *&out, bool &removeSession);

    /**
     * When data received, will call this method.
     * @param[in] in             - the in data.
     * @param[out] out           - the out data.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Recv(void *in, void *&out, bool &removeSession);

    /**
     * Add coder factory to protocol, only available in Codec-Layer.
     * @param[in] opcode - the opcode.
     * @param[in] coder  - the coder factory
     *  Note: When protocol deleted, it will not delete coder pointer,
     *        It means that you must self manage your coder memory.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int AddCoder(int opcode, LLBC_CoderFactory *coder);
};

__LLBC_NS_END
