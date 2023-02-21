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
#include "pyllbc/common/PyStream.h"

/**
 * \brief The pyllbc Pack-Lemma class encapsulation.
 */
class LLBC_HIDDEN pyllbc_PackLemma
{
    typedef pyllbc_PackLemma This;
public:
    /**
     * The lemma type enumeration.
     */
    enum Type
    {
        TopType,
        RawType,
        SequenceType,
        DictType,
        ClassType,

        InvalidType
    };

    /**
     * The lemma symbols enumeration.
     */
    enum Symbol
    {
        Byte = 'b',

        Boolean = 'B',

        Int16 = 's',
        Int32 = 'i',
        Int64 = 'q',
        Float = 'f',
        Double = 'd',

        String = 'S',
        StringLen = '#',
        Unicode = 'U',

        ByteArray = 'A',
        Buffer = 'F',

        Sequence = 'Q',
        None = 'N',

        Class = 'C',

        TupleBegin = '(',
        TupleEnd = ')',
        ListBegin = '[',
        ListEnd = ']',
        DictBegin = '{',
        DictEnd = '}',
        DictKWSep = ':',
        ClassNameBegin = '<',
        ClassNameEnd = '>',

        InvalidSymbol = '\0'
    };

    /**
     * The Pack-Lemma state type enumeration.
     */
    enum State
    {
        Begin,
        Accepting,
        Done,
        Error
    };

public:
    /**
     * The symbol group typedef.
     */
    typedef std::set<Symbol> SymbolGroup;

    /**
     * \brief The symbol group helper class.
     */
    class LLBC_HIDDEN GroupedSymbol
    {
    public:
        /**
         * Get the RAW group symbols.
         * @return const SymbolGroup & - the raw group symbols.
         */
        static const SymbolGroup &Raw();

        /**
         * Get the Sequence group begin/end symbols.
         * @return const SymbolGroup & - the sequence group begin/end symbols.
         */
        static const SymbolGroup &SeqBegin();
        static const SymbolGroup &SeqEnd();

        /**
         * Get the list/tuple begin&end pair symbols.
         * @return const SymbolGoup & - the list/tuple begin&end pair symbols.
         */
        static const SymbolGroup &ListPair();
        static const SymbolGroup &TuplePair();

    private:
        static SymbolGroup _raw;

        static SymbolGroup _seqBegin;
        static SymbolGroup _seqEnd;

        static SymbolGroup _listPair;
        static SymbolGroup _tuplePair;
    };

public:
    /**
     * Constructor & Destructor.
     */
    pyllbc_PackLemma(PyObject *compileEnv = nullptr);
    virtual ~pyllbc_PackLemma();

public:
    /**
     * Get lemma type, see Type enumeration.
     * @return Type - the lemma type enumeration.
     */
    virtual Type GetType() const = 0;
    /**
     * Get lemma current state, see State enumeration.
     * @return State - the lemma state enumeration.
     */
    virtual State GetState() const;

    /**
     * Check lemma is serializable(Readable & Writable) or not.
     * @return bool - return true f serializable, otherwise return false.
     */
    virtual bool IsSerializable() const = 0;

    /**
     * Check lemma is parse done or not.
     * @return bool - return - true if done, otherwise not done or maybe error occurred.
     */
    virtual bool IsDone() const;

    /**
     * Check lemma error occurred or not.
     * @return bool - true if error occurred, otherwise return false.
     */
    virtual bool IsErrorOccurred() const;

    /**
     * Process character sequence.
     * @param[in] ch     - the character symbol.
     * @param[in] nextCh - the next character symbol(can use it to reference).
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Process(Symbol ch, Symbol nextCh = This::InvalidSymbol) = 0;
    /**
     * Process lemma.
     * @param[in] lemma - the lemma.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Process(This *lemma) = 0;

    /**
     * Read data from pystream according lemma format.
     * @param[in] stream - the pystream.
     * @return PyObject * - the readed python layer object, new reference, if error occurred return nullptr.
     */
    virtual PyObject *Read(pyllbc_Stream *stream) = 0;
    /**
     * rite python layer data to given string according lemma format.
     * @param[in] stream - the pystream.
     * @param[in] values - the python layer values, must be tuple or None, if error occurred return nullptr.
     */
    virtual int Write(pyllbc_Stream *stream, PyObject *values) = 0;

    /**
     * Convert lemma to human readable string.
     * @return const LLBC_String & - the string.
     */
    virtual const LLBC_String &ToString() const;

protected:
    State _state;
    LLBC_String _str;
    LLBC_String _emptyStr;

    PyObject *_compileEnv;
};
