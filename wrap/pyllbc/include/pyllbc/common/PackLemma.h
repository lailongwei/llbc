/**
 * @file    PackLemma.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/09/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_PACK_LEMMA_H__
#define __PYLLBC_COM_PACK_LEMMA_H__

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
    pyllbc_PackLemma(PyObject *compileEnv = NULL);
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
     * @return PyObject * - the readed python layer object, new reference, if error occurred return NULL.
     */
    virtual PyObject *Read(pyllbc_Stream *stream) = 0;
    /**
     * rite python layer data to given string according lemma format.
     * @param[in] stream - the pystream.
     * @param[in] values - the python layer values, must be tuple or None, if error occurred return NULL.
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

#endif // !__PYLLBC_COM_PACK_LEMMA_H__
