/**
* @file    _Random.h
* @author  Longwei Lai<lailongwei@126.com>
* @date    2014/08/14
* @version 1.0
*
* @brief
*/

#include "pyllbc/common/Common.h"

namespace 
{
    inline PyObject *__Rand1Int(LLBC_NS sint32 val)
    {
        if (val > 0)
            return Py_BuildValue("I", LLBC_Random::RandInt32(val));
        else if (val < 0)
            return Py_BuildValue("i", -static_cast<LLBC_NS sint32>(LLBC_Random::RandInt32(-val)));
        else
            return Py_BuildValue("i", 0);
    }

    inline PyObject *__Rand2Int(LLBC_NS sint32 m, LLBC_NS sint32 n)
    {
        if (m > n)
            LLBC_Swap(m, n);
        else if (m == n)
            return Py_BuildValue("I", m);

        if (m >= 0)
            return Py_BuildValue("I", LLBC_Random::RandInt32cmcn(m, n));
        else
            return Py_BuildValue("i", static_cast<LLBC_NS sint32>(LLBC_Random::RandInt32cmcn(0, n - m)) + m);
    }
}

LLBC_EXTERN_C PyObject *_pyllbc_seed(PyObject *self, PyObject *args)
{
    double seed;
    if (!PyArg_ParseTuple(args, "d", &seed))
        return NULL;

    LLBC_Random::Seed(static_cast< ::llbc::ulong>(seed));

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_rand(PyObject *self, PyObject *args)
{
    PyObject *obj1 = NULL;
    PyObject *obj2 = NULL;
    if (!PyArg_ParseTuple(args, "|OO", &obj1, &obj2))
        return NULL;

    const bool isObj1None = !obj1 || PyObject_IsInstance(obj1, PYLLBC_NONE_CLS);
    const bool isObj1Int = obj1 && PyObject_IsInstance(obj1, PYLLBC_INT_CLS);
    const bool isObj1Long = isObj1Int || (obj1 && PyObject_IsInstance(obj1, PYLLBC_LONG_CLS));
    const bool isObj1Float = obj1 && PyObject_IsInstance(obj1, PYLLBC_FLOAT_CLS);

    const bool isObj2None = !obj2 || PyObject_IsInstance(obj2, PYLLBC_NONE_CLS);
    const bool isObj2Int = obj2 && PyObject_IsInstance(obj2, PYLLBC_INT_CLS);
    const bool isObj2Long = isObj2Int || (obj2 && PyObject_IsInstance(obj2, PYLLBC_LONG_CLS));
    const bool isObj2Float = obj2 && PyObject_IsInstance(obj2, PYLLBC_FLOAT_CLS);

    if ((!isObj1None && !isObj1Int && !isObj1Long && !isObj1Float) ||
        (!isObj2None && !isObj2Int && !isObj2Long && !isObj2Float))
    {
        pyllbc_SetError("rand arg type error", PYLLBC_ERROR_COMMON, PyExc_TypeError);
        return NULL;
    }

    if (isObj1None)
    {
        if (isObj2None)
        {
            return Py_BuildValue("I", LLBC_Random::RandInt32());
        }
        else if (isObj2Long)
        {
            LLBC_NS sint32 n;
            if (!PyArg_Parse(obj2, "i", &n))
                return NULL;

            return __Rand1Int(n);
        }
        else
        {
            double n;
            if (!PyArg_Parse(obj2, "d", &n))
                return NULL;

            return Py_BuildValue("d", LLBC_Random::RandRealc0cn(n));
        }
    }
    else if (isObj1Long)
    {
        LLBC_NS sint32 m;
        if (!PyArg_Parse(obj1, "i", &m))
            return NULL;

        if (isObj2None)
        {
            return __Rand1Int(m);
        }
        else if (isObj2Long)
        {
            LLBC_NS sint32 n;
            if (!PyArg_Parse(obj2, "i", &n))
                return NULL;

            return __Rand2Int(m, n);
        }
        else
        {
            double dblN;
            if (!PyArg_Parse(obj2, "d", &dblN))
                return NULL;

            const LLBC_NS sint32 n = static_cast<LLBC_NS sint32>(dblN);

            return __Rand2Int(m, n);
        }
    }
    else
    {
        double m;
        if (!PyArg_Parse(obj1, "d", &m))
            return NULL;

        if (isObj2None)
        {
            return Py_BuildValue("d", LLBC_Random::RandRealc0cn(m));
        }
        else if (isObj2Long)
        {
            uint32 n;
            if (!PyArg_Parse(obj2, "I", &n))
                return NULL;

            return __Rand2Int(static_cast<uint32>(m), n);
        }
        else
        {
            double dblN;
            if (!PyArg_Parse(obj2, "d", &dblN))
                return NULL;

            const LLBC_NS sint32 n = static_cast<LLBC_NS sint32>(dblN);

            return __Rand2Int(static_cast<LLBC_NS sint32>(m), n);
        }
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_rand53real(PyObject *self, PyObject *args)
{
    return Py_BuildValue("d", LLBC_Random::Rand53Real());
}

LLBC_EXTERN_C PyObject *_pyllbc_choice(PyObject *self, PyObject *args)
{
    PyObject *seq;
    if (!PyArg_ParseTuple(args, "O", &seq))
        return NULL;

    if (PySequence_Check(seq))
    {
#if _M_X64
        const int size = static_cast<int>(PySequence_Size(seq));
#else
        const int size = PySequence_Size(seq);
#endif
        if (size == 0)
        {
            pyllbc_SetError("sequence index out of range", PYLLBC_ERROR_COMMON, PyExc_IndexError);
            return NULL;
        }

        return PySequence_GetItem(seq, LLBC_Random::RandInt32cmon(0, size));
    }

    pyllbc_SetError("could not choice, arg not sequence", 
        PYLLBC_ERROR_COMMON, 
        PyExc_ValueError);
    return NULL;
}

LLBC_EXTERN_C PyObject *_pyllbc_shuffle(PyObject *self, PyObject *args)
{
    PyObject *seq;
    if (!PyArg_ParseTuple(args, "O", &seq))
        return NULL;

    if (!PySequence_Check(seq))
    {
        pyllbc_SetError("could not shuffle, arg not mutable sequence");
        return NULL;
    }

    const size_t size = static_cast<size_t>(PySequence_Size(seq));
    if (size < 2)
        Py_RETURN_NONE;

    Py_INCREF(seq);

    typedef std::vector<size_t> _Idxs;
    _Idxs idxs;
    do {
        size_t randIdx = 0;
        do {
            randIdx = LLBC_Random::RandInt32cmon(0, static_cast<uint32>(size));
        } while(std::find(idxs.begin(), idxs.end(), randIdx) != idxs.end());

        idxs.push_back(randIdx);
    } while (idxs.size() != size);

    typedef std::vector<PyObject *> _Objs;

    _Objs copyObjs;
    copyObjs.resize(size);
    for (size_t i = 0; i < size; i++)
        copyObjs[i] = PySequence_GetItem(seq, i);

    for (size_t i = 0; i < size; i++)
    {
        PyObject *&obj = copyObjs[idxs[i]];
        PySequence_SetItem(seq, i, obj);

        Py_DECREF(obj);
    }

    Py_DECREF(seq);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_booljudge(PyObject *self, PyObject *args)
{
    PyObject *judgeResult = LLBC_Random::RandInt32cmcn(0, 1) ? Py_True : Py_False;
    Py_INCREF(judgeResult);

    return judgeResult;
}
