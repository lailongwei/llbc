/**
 * @file    PackLemmaBuilder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_PACK_LEMMA_BUILDER_H__
#define __PYLLBC_COM_PACK_LEMMA_BUILDER_H__

#include "pyllbc/common/LibHeader.h"
#include "pyllbc/common/PackLemma.h"

/**
 * \brief The pack-lemma builder class encapsulation.
 */
class LLBC_HIDDEN pyllbc_PackLemmaBuilder
{
public:
    /**
     * Create pack-lemma by type.
     * @param[in] type       - the pack-lemma type.
     * @param[in] compileEnv - the compile environment.
     * @return pyllbc_PackLemma * - the pack-lemma object.
     */
    static pyllbc_PackLemma *Build(pyllbc_PackLemma::Type type, PyObject *compileEnv = NULL);
};

#endif // !__PYLLBC_COM_PACK_LEMMA_BUILDER_H__
