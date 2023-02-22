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

class pyllbc_PackLemma;

/**
 * \brief The pack-lemma compiler class encapsulation.
 *        Note: This compiler also buff pack-lemma.
 */
class LLBC_HIDDEN pyllbc_PackLemmaCompiler
{
    typedef pyllbc_PackLemmaCompiler This;
public:
    /**
     * The pack character format semantic enumeration.
     */
    enum Semantic
    {
        SeqBegin,
        SeqEnd,

        DictBegin,
        DictEnd,
        DictKWSep,

        Class,
        ClassName,
        ClassNameBegin,
        ClassNameEnd,

        Raw,

        Invalid
    };

public:
    /**
     * Constructor & Destructor.
     */
    pyllbc_PackLemmaCompiler();
    virtual ~pyllbc_PackLemmaCompiler();

public:
    /**
     * Compile pack-lemma string expression.
     * @param[in] expr      - the expression.
     * @param[in] force     - indicate compiler force compile expression or use cached expression(if has), default is false.
     * @param[in] callerEnv - the compile environment.
     * @return pyllbc_PackLemma * - the compiled pack-lemma object.
     */
    pyllbc_PackLemma *Compile(const LLBC_String &expr, bool force = false, PyObject *compileEnv = nullptr);

    /**
     * Discard specific expression pack-lemma object in compiler.
     * @param[in] expr - the expression.
     */
    void DiscardCache(const LLBC_String &expr);

    /**
     * Discard compiler's all cached pack-lemma objects.
     */
    void DiscardAllCaches();

    /**
     * Get the pack-lemma buff cache limit.
     * @return size_t - the cache limit value, default is 1000.
     */
    size_t GetCacheLimit() const;

    /**
     * Set the pack-lemma buff cache limit.
     * @param[in] newSize - adjust the cache size.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetCacheLimit(size_t newSize);

    /**
     * Get cached size.
     * @return size_t - the cached size.
     */
    size_t GetCachedSize() const;

    /**
     * Check given expression is compiled or not.
     * @param[in] expr - the format expression.
     * @return bool - return true if success, otherwise return -1.
     */
    bool IsExprCompiled(const LLBC_String &expr) const;

private:
    /**
     * Reduce compiling stack.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReduceStack();

    /**
     * Compile character.
     * @param[in] ch     - the character.
     * @param[in] nextCh - the next character.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CompileChar(char ch, char nextCh, PyObject *compileEnv);
    int Compile_SeqBegin(char ch, char nextCh, PyObject *compileEnv);
    int Compile_SeqEnd(char ch, char nextCh, PyObject *compileEnv);
    int Compile_DictBegin(char ch, char nextCh, PyObject *compileEnv);
    int Compile_DictEnd(char ch, char nextCh, PyObject *compileEnv);
    int Compile_DictKWSep(char ch, char nextCh, PyObject *compileEnv);
    int Compile_Class(char ch, char nextCh, PyObject *compileEnv);
    int Compile_ClassNameBeg(char ch, char nextCh, PyObject *compileEnv);
    int Compile_ClassNameEnd(char ch, char nextCh, PyObject *compileEnv);
    int Compile_ClassName(char ch, char nextCh, PyObject *compileEnv);
    int Compile_Raw(char ch, char nextCh, PyObject *compileEnv);
    int Compile_Invalid(char ch, char nextCh, PyObject *compileEnv);

    /**
     * Resolve the character semantic according to the compiling stack.
     * @param[in] ch - the pack format character.
     * @return Semantic - the semantic type enumerate value, if failed, return Invalid.
     *                    about semantic type, see Semantic enumeration.
     */
    Semantic ResolveSemantic(char ch);

    /**
     * Remove the oldest expression.
     */
    void RemoveOldestExpr();

private:
    typedef std::map<LLBC_String, pyllbc_PackLemma *> _Exprs;
    typedef std::deque<LLBC_String> _ExprsIdxQueue;
    _Exprs _exprs;
    _ExprsIdxQueue _exprsIdxQueue;
    
    size_t _cacheLimit;

    std::stack<pyllbc_PackLemma *> _compilingStack;
    int (This::*_compMeths[This::Invalid + 1])(char, char, PyObject *);
};

// Singleton macro define.
#define pyllbc_s_PackLemmaCompiler LLBC_Singleton<pyllbc_PackLemmaCompiler>::Instance()


