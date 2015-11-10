/**
 * @file    PackLemmaCompiler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/03
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_PACK_LEMMA_COMPILER_H__
#define __PYLLBC_COM_PACK_LEMMA_COMPILER_H__

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
    pyllbc_PackLemma *Compile(const LLBC_String &expr, bool force = false, PyObject *compileEnv = NULL);

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

#endif // !__PYLLBC_COM_PACK_LEMMA_COMPILER_H__
