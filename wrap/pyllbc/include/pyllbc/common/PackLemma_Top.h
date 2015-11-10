/**
 * @file    PackLemma_Top.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_PACK_LEMMA_TOP_H__
#define __PYLLBC_COM_PACK_LEMMA_TOP_H__

#include "pyllbc/common/LibHeader.h"
#include "pyllbc/common/PackLemma.h"

/**
 * \brief The top type pack-lemma class encapsulation.
 */
class LLBC_HIDDEN pyllbc_PackLemma_Top : public pyllbc_PackLemma
{
    typedef pyllbc_PackLemma Base;
public:
    /**
     * Constructor & Destructor.
     */
    pyllbc_PackLemma_Top(PyObject *compileEnv = NULL);
    virtual ~pyllbc_PackLemma_Top();

public:
    /**
     * Get lemma type, see Type enumeration.
     * @return Type - the lemma type enumeration.
     */
    virtual Type GetType() const;
    /**
     * Get lemma current state, see State enumeration.
     * @return State - the lemma state enumeration.
     */
    virtual State GetState() const;

    /**
     * Check lemma is serializable(Readable & Writable) or not.
     * @return bool - return true f serializable, otherwise return false.
     */
    virtual bool IsSerializable() const;

    /**
     * Check lemma is parse done or not.
     * @return bool - return - true if done, otherwise not done or maybe error occurred.
     */
    virtual bool IsDone() const;
    
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
     * @return PyObject * - the readed python layer object, new reference, if error occurred return NULL.
     */
    virtual PyObject *Read(pyllbc_Stream *stream);
    /**
     * rite python layer data to given string according lemma format.
     * @param[in] stream - the pystream.
     * @param[in] values - the python layer values, must be tuple or None, if error occurred return NULL.
     */
    virtual int Write(pyllbc_Stream *stream, PyObject *values);

private:
    std::vector<Base *> _lemmas;
};

#endif // !__PYLLBC_COM_PACK_LEMMA_TOP_H__
