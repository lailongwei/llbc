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
        return new pyllbc_PackLemma_Raw(compileEnv);

    case PackLemma::TopType:
        return new pyllbc_PackLemma_Top(compileEnv);

    case PackLemma::SequenceType:
        return new pyllbc_PackLemma_Sequence(compileEnv);

    case PackLemma::DictType:
        return new pyllbc_PackLemma_Dict(compileEnv);

    case PackLemma::ClassType:
        return new pyllbc_PackLemma_Class(compileEnv);
        break;

    default:
        break;
    }

    return nullptr;
}
