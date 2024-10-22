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

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/PackLemmaBuilder.h"
#include "pyllbc/common/PackLemmaCompiler.h"

namespace
{
    typedef pyllbc_PackLemma Type;
    typedef pyllbc_PackLemma::Symbol Symbol;

    typedef pyllbc_PackLemma::SymbolGroup SymbolGroup;
    typedef pyllbc_PackLemma::GroupedSymbol GroupedSymbol;

    typedef pyllbc_PackLemmaCompiler This;
    typedef This::Semantic Semantic;
}

pyllbc_PackLemmaCompiler::pyllbc_PackLemmaCompiler()
{
    _cacheLimit = 1000;

    _compMeths[This::SeqBegin] = &This::Compile_SeqBegin;
    _compMeths[This::SeqEnd] = &This::Compile_SeqEnd;

    _compMeths[This::DictBegin] = &This::Compile_DictBegin;
    _compMeths[This::DictEnd] = &This::Compile_DictEnd;
    _compMeths[This::DictKWSep] = &This::Compile_DictKWSep;

    _compMeths[This::Class] = &This::Compile_Class;
    _compMeths[This::ClassNameBegin] = &This::Compile_ClassNameBeg;
    _compMeths[This::ClassNameEnd] = &This::Compile_ClassNameEnd;
    _compMeths[This::ClassName] = &This::Compile_ClassName;

    _compMeths[This::Raw] = &This::Compile_Raw;

    _compMeths[This::Invalid] = &This::Compile_Invalid;
}

pyllbc_PackLemmaCompiler::~pyllbc_PackLemmaCompiler()
{
    LLBC_STLHelper::DeleteContainer(_exprs);
}

void pyllbc_PackLemmaCompiler::DiscardCache(const LLBC_String &expr)
{
    _Exprs::iterator it = _exprs.find(expr);
    if (it == _exprs.end())
        return;

    for (_ExprsIdxQueue::iterator idxIt = _exprsIdxQueue.begin();
         idxIt != _exprsIdxQueue.end();
         idxIt++)
        if (*idxIt == expr)
        {
            _exprsIdxQueue.erase(idxIt);
            break;
        }

    delete it->second;
    _exprs.erase(it);
}

void pyllbc_PackLemmaCompiler::DiscardAllCaches()
{
    _exprsIdxQueue.clear();
    LLBC_STLHelper::DeleteContainer(_exprs);
}

size_t pyllbc_PackLemmaCompiler::GetCacheLimit() const
{
    return _cacheLimit;
}

int pyllbc_PackLemmaCompiler::SetCacheLimit(size_t newSize)
{
    if (newSize == 0)
    {
        pyllbc_SetError("could not set pack-lemma compiler cache limit to 0");
        return LLBC_FAILED;
    }

    _cacheLimit = newSize;
    while (GetCachedSize() > _cacheLimit)
        RemoveOldestExpr();

    return LLBC_OK;
}

size_t pyllbc_PackLemmaCompiler::GetCachedSize() const
{
    return _exprs.size();
}

bool pyllbc_PackLemmaCompiler::IsExprCompiled(const LLBC_String &expr) const
{
    return (_exprs.find(expr) != _exprs.end()) ? true : false;
}

pyllbc_PackLemma *pyllbc_PackLemmaCompiler::Compile(const LLBC_String &expr, bool force, PyObject *compileEnv)
{
    if (UNLIKELY(expr.empty()))
    {
        pyllbc_SetError("could not compile empty pack format characters");
        return nullptr;
    }

    if (LIKELY(!force))
    {
        _Exprs::iterator it = _exprs.find(expr);
        if (it != _exprs.end())
            return it->second;
    }
    else
    {
        DiscardCache(expr);
    }

    _compilingStack.push(
        pyllbc_PackLemmaBuilder::Build(Type::TopType, compileEnv));

    const char *str = expr.c_str();
    const LLBC_String::size_type len = expr.length();
    for (LLBC_String::size_type i = 0; i < len; ++i)
    {
        char ch = str[i];
        char nextCh = i + 1 < len ? str[i + 1] : '\0';

        if (CompileChar(ch, nextCh, compileEnv) != LLBC_OK)
        {
            LLBC_STLHelper::DeleteContainer(_compilingStack);
            return nullptr;
        }

        if (ReduceStack() != LLBC_OK)
        {
            LLBC_STLHelper::DeleteContainer(_compilingStack);
            return nullptr;
        }
    }

    if (_compilingStack.size() > 1)
    {
        LLBC_STLHelper::DeleteContainer(_compilingStack);
        pyllbc_SetError("pack string format error, maybe missing close character') ] > }'");

        return nullptr;
    }

    pyllbc_PackLemma *lemma = _compilingStack.top();
    _compilingStack.pop();

    if (_exprs.size() == _cacheLimit)
        RemoveOldestExpr();

    _exprsIdxQueue.push_back(expr);
    _exprs.insert(std::make_pair(expr, lemma));

    return lemma;
}

int pyllbc_PackLemmaCompiler::ReduceStack()
{
    while (_compilingStack.size() > 1)
    {
        pyllbc_PackLemma *lemma = _compilingStack.top();
        if (!lemma->IsDone())
            break;

        _compilingStack.pop();
        if (_compilingStack.top()->Process(lemma) != LLBC_OK)
        {
            delete lemma;
            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int pyllbc_PackLemmaCompiler::CompileChar(char ch, char nextCh, PyObject *compileEnv)
{
    Semantic sematic = ResolveSemantic(ch);
    return (this->*_compMeths[sematic])(ch, nextCh, compileEnv);
}

int pyllbc_PackLemmaCompiler::Compile_SeqBegin(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *lemma = 
        pyllbc_PackLemmaBuilder::Build(Type::SequenceType, compileEnv);
    if (lemma->Process(static_cast<Symbol>(ch)) != LLBC_OK)
    {
        delete lemma;
        return LLBC_FAILED;
    }

    _compilingStack.push(lemma);
    return LLBC_OK;
}

int pyllbc_PackLemmaCompiler::Compile_SeqEnd(char ch, char nextCh, PyObject *compileEnv)
{
    const size_t stackSize = _compilingStack.size();
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if (stackSize == 1 || 
        topLemma->GetType() != Type::SequenceType)
    {
        LLBC_String errStr;
        errStr.format("found sequence end format character: %c, but before this "
            "not got sequence begin character: %c/%c", ch, pyllbc_PackLemma::ListBegin, pyllbc_PackLemma::TupleBegin);

        pyllbc_SetError(errStr);

        return LLBC_FAILED;
    }

    return topLemma->Process(static_cast<Symbol>(ch));
}

int pyllbc_PackLemmaCompiler::Compile_DictBegin(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *lemma =
        pyllbc_PackLemmaBuilder::Build(Type::DictType, compileEnv);
    if (lemma->Process(static_cast<Symbol>(ch)) != LLBC_OK)
    {
        delete lemma;
        return LLBC_FAILED;
    }

    _compilingStack.push(lemma);
    return LLBC_OK;
}

int pyllbc_PackLemmaCompiler::Compile_DictEnd(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if (topLemma->GetType() != Type::DictType)
    {
        LLBC_String errStr;
        errStr.format("found dict end format character: %c, "
            "but before this not got dict begin character: %c", ch, pyllbc_PackLemma::DictBegin);

        pyllbc_SetError(errStr);

        return LLBC_FAILED;
    }

    return topLemma->Process(static_cast<Symbol>(ch));
}

int pyllbc_PackLemmaCompiler::Compile_DictKWSep(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if (topLemma->GetType() != Type::DictType)
    {
        LLBC_String errStr;
        errStr.format("found dict key-word separator character: %c, "
            "but before this not got dict begin character: %c", ch, pyllbc_PackLemma::DictBegin);

        pyllbc_SetError(errStr);

        return LLBC_FAILED;
    }

    return topLemma->Process(static_cast<Symbol>(ch));
}

int pyllbc_PackLemmaCompiler::Compile_Class(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *lemma = 
        pyllbc_PackLemmaBuilder::Build(Type::ClassType, compileEnv);
    if (lemma->Process(static_cast<Symbol>(ch)) != LLBC_OK)
    {
        delete lemma;
        return LLBC_FAILED;
    }

    if (nextCh != static_cast<char>(pyllbc_PackLemma::ClassNameBegin))
    {
        if (lemma->Process(pyllbc_PackLemma::ClassNameBegin) != LLBC_OK ||
            lemma->Process(pyllbc_PackLemma::ClassNameEnd) != LLBC_OK)
            return LLBC_FAILED;
    }

    _compilingStack.push(lemma);
    return LLBC_OK;
}

int pyllbc_PackLemmaCompiler::Compile_ClassNameBeg(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if (topLemma->GetType() != Type::ClassType)
    {
        LLBC_String errStr;
        errStr.format("found class name begin character: %c, "
            "but before this not got class format character: %c", ch, pyllbc_PackLemma::Class);

        pyllbc_SetError(errStr);

        return LLBC_FAILED;
    }

    return topLemma->Process(static_cast<Symbol>(ch));
}

int pyllbc_PackLemmaCompiler::Compile_ClassNameEnd(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if (topLemma->GetType() != Type::ClassType)
    {
        LLBC_String errStr;
        errStr.format("found class name end character: %c, "
            "but before this not got class name begin format character: %c", ch, pyllbc_PackLemma::ClassNameBegin);

        pyllbc_SetError(errStr);

        return LLBC_FAILED;
    }

    return topLemma->Process(static_cast<Symbol>(ch));
}

int pyllbc_PackLemmaCompiler::Compile_ClassName(char ch, char nextCh, PyObject *compileEnv)
{
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if (topLemma->GetType() != Type::ClassType)
    {
        LLBC_String errStr;
        errStr.format("found class name character: %c, "
            "but before this not got class name format character: %c", ch, pyllbc_PackLemma::Class);

        pyllbc_SetError(errStr);

        return LLBC_FAILED;
    }

    return topLemma->Process(static_cast<Symbol>(ch));
}

int pyllbc_PackLemmaCompiler::Compile_Raw(char ch, char nextCh, PyObject *compileEnv)
{
    const Symbol &smbl = static_cast<Symbol>(ch);
    pyllbc_PackLemma *topLemma = _compilingStack.top();
    if ((smbl == Type::StringLen) &&
        topLemma->GetType() == Type::RawType)
        return topLemma->Process(smbl, static_cast<Symbol>(nextCh));

    pyllbc_PackLemma *lemma = pyllbc_PackLemmaBuilder::Build(Type::RawType, compileEnv);
    if (lemma->Process(smbl, static_cast<Symbol>(nextCh)) != LLBC_OK)
    {
        delete lemma;
        return LLBC_FAILED;
    }

    _compilingStack.push(lemma);
    return LLBC_OK;
}

int pyllbc_PackLemmaCompiler::Compile_Invalid(char ch, char nextCh, PyObject *compileEnv)
{
    LLBC_String errStr;
    pyllbc_SetError(errStr.format("invalid pack format character: %c", ch));

    return LLBC_FAILED;
}

Semantic pyllbc_PackLemmaCompiler::ResolveSemantic(char ch)
{
    const Symbol smbl = static_cast<Symbol>(ch);

    // SeqBeg, SeqEnd.
    const SymbolGroup &seqBeg = GroupedSymbol::SeqBegin();
    if (seqBeg.find(smbl) != seqBeg.end())
        return This::SeqBegin;

    const SymbolGroup &seqEnd = GroupedSymbol::SeqEnd();
    if (seqEnd.find(smbl) != seqEnd.end())
        return This::SeqEnd;

    // DictBeg, DictEnd, DictKWSep.
    if (smbl == pyllbc_PackLemma::DictBegin)
        return This::DictBegin;
    else if (smbl == pyllbc_PackLemma::DictEnd)
        return This::DictEnd;
    else if (smbl == pyllbc_PackLemma::DictKWSep)
        return This::DictKWSep;

    // Class.
    const size_t stackSize = _compilingStack.size();
    const pyllbc_PackLemma::Type topLemmaType = _compilingStack.top()->GetType();

    if (smbl == pyllbc_PackLemma::Class && 
        topLemmaType != Type::ClassType)
        return This::Class;

    // ClassNameBeg, ClassNameEnd, ClassName.
    if (smbl == pyllbc_PackLemma::ClassNameBegin)
        return This::ClassNameBegin;
    else if (smbl == pyllbc_PackLemma::ClassNameEnd)
        return This::ClassNameEnd;
    else if (stackSize > 1)
        if (topLemmaType == Type::ClassType)
            return This::ClassName;

    // Raw.
    const SymbolGroup &raw = GroupedSymbol::Raw();
    if (raw.find(smbl) != raw.end())
        return This::Raw;

    // Invalid.
    return This::Invalid;
}

void pyllbc_PackLemmaCompiler::RemoveOldestExpr()
{
    if (UNLIKELY(_exprs.empty()))
        return;

    const LLBC_String &id = _exprsIdxQueue.front();

    _Exprs::iterator it = _exprs.find(id);
    delete it->second;
    _exprs.erase(it);

    _exprsIdxQueue.pop_front();
}
