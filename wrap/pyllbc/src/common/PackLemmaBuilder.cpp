/**
 * @file    PackLemmaBuilder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/01
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/PackLemma_Raw.h"
#include "pyllbc/common/PackLemma_Top.h"
#include "pyllbc/common/PackLemma_Sequence.h"
#include "pyllbc/common/PackLemma_Dict.h"
#include "pyllbc/common/PackLemma_Class.h"

#include "pyllbc/common/PackLemmaBuilder.h"

namespace
{
    typedef pyllbc_PackLemma PackLemma;
    typedef PackLemma::Type Type;
}

pyllbc_PackLemma *pyllbc_PackLemmaBuilder::Build(Type type, PyObject *compileEnv)
{
    switch (type)
    {
    case PackLemma::RawType:
        return LLBC_New1(pyllbc_PackLemma_Raw, compileEnv);

    case PackLemma::TopType:
        return LLBC_New1(pyllbc_PackLemma_Top, compileEnv);

    case PackLemma::SequenceType:
        return LLBC_New1(pyllbc_PackLemma_Sequence, compileEnv);

    case PackLemma::DictType:
        return LLBC_New1(pyllbc_PackLemma_Dict, compileEnv);

    case PackLemma::ClassType:
        return LLBC_New1(pyllbc_PackLemma_Class, compileEnv);
        break;

    default:
        break;
    }

    return NULL;
}
