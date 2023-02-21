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
#include "pyllbc/common/PackLemma.h"

/**
 * \brief The sequence type pack-lemma class encapsulation.
 */
class LLBC_HIDDEN pyllbc_PackLemma_Sequence : public pyllbc_PackLemma
{
    typedef pyllbc_PackLemma Base;
public:
    /**
     * Constructor & Destructor.
     */
    pyllbc_PackLemma_Sequence(PyObject *compileEnv = nullptr);
    virtual ~pyllbc_PackLemma_Sequence();

public:
    /**
     * Get lemma type, see Type enumeration.
     * @return Type - the lemma type enumeration.
     */
    virtual Type GetType() const;

    /**
     * Check lemma is serializable(Readable & Writable) or not.
     * @return bool - return true f serializable, otherwise return false.
     */
    virtual bool IsSerializable() const;

    /**
     * Process character sequence.
     * @param[in] ch     - the character symbol.
     * @param[in] nextCh - the next character symbol(can use it to reference).
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Process(Symbol ch, Symbol nextCh = Base::InvalidSymbol);
    /**
     * Process lemma.
     * @param[in] lemma - the lemma.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Process(Base *lemma);

    /**
     * Read data from pystream according lemma format.
     * @param[in] stream - the pystream.
     * @return PyObject * - the readed python layer object, new reference, if error occurred return nullptr.
     */
    virtual PyObject *Read(pyllbc_Stream *stream);
    /**
     * rite python layer data to given string according lemma format.
     * @param[in] stream - the pystream.
     * @param[in] values - the python layer values, must be tuple or None, if error occurred return nullptr.
     */
    virtual int Write(pyllbc_Stream *stream, PyObject *values);

private:
    Symbol _seqType;
    std::vector<Base *> _lemmas;
};
