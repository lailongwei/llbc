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

#include "pyllbc/common/Macro.h"
#include "pyllbc/common/Errors.h"
#include "pyllbc/common/PackLemma_Class.h"

namespace
{
    typedef pyllbc_PackLemma Base;

    typedef Base::Type Type;
    typedef Base::State State;
    typedef Base::Symbol Symbol;
}

std::map<LLBC_String, PyObject *> pyllbc_PackLemma_Class::_rawClasses;

pyllbc_PackLemma_Class::pyllbc_PackLemma_Class(PyObject *compileEnv)
: Base(compileEnv)
, _gotLeftAngle(false)

, _clsName()
, _class(nullptr)
{
    if (UNLIKELY(_rawClasses.empty()))
    {
        _rawClasses.insert(std::make_pair("bool", PYLLBC_BOOL_CLS));
        _rawClasses.insert(std::make_pair("int", PYLLBC_INT_CLS));
        _rawClasses.insert(std::make_pair("long", PYLLBC_LONG_CLS));
        _rawClasses.insert(std::make_pair("str", PYLLBC_STR_CLS));
        _rawClasses.insert(std::make_pair("float", PYLLBC_FLOAT_CLS));
        _rawClasses.insert(std::make_pair("NoneType", PYLLBC_NONE_CLS));
        _rawClasses.insert(std::make_pair("bytearray", PYLLBC_BYTEARRAY_CLS));
        _rawClasses.insert(std::make_pair("buffer", PYLLBC_BUFFER_CLS));
    }
}

pyllbc_PackLemma_Class::~pyllbc_PackLemma_Class()
{
    Py_XDECREF(_class);
}

Type pyllbc_PackLemma_Class::GetType() const
{
    return Base::ClassType;
}

bool pyllbc_PackLemma_Class::IsSerializable() const
{
    return true;
}

int pyllbc_PackLemma_Class::Process(Symbol ch, Symbol nextCh)
{
    if (_state == Base::Done || _state == Base::Error)
    {
        pyllbc_SetError("class-lemma could continuing accept format character, lemma done or error occurred");
        return LLBC_FAILED;
    }

    if (_state == Base::Begin)
    {
        if (ch != Base::Class)
        {
            _state = Base::Error;
            pyllbc_SetError("class-lemma expect class format character to start, got: %c", ch);

            return LLBC_FAILED;
        }

        _state = Base::Accepting;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }

    if (!_gotLeftAngle)
    {
        if (ch != Base::ClassNameBegin)
        {
            _state = Base::Error;
            pyllbc_SetError("class-lemma expect class name begin format character, got: %c", ch);

            return LLBC_FAILED;
        }

        _gotLeftAngle = true;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }

    if (ch == Base::ClassNameEnd)
    {
        if (!_clsName.empty())
        {
            std::map<LLBC_String, PyObject *>::iterator clsIt = _rawClasses.find(_clsName);
            if (clsIt != _rawClasses.end())
            {
                _class = clsIt->second;
                Py_INCREF(_class);
            }
            else
            {
                if (!_compileEnv)
                {
                    _state = Base::Error;
                    pyllbc_SetError("not set compile environment to compile class pack-lemma");

                    return LLBC_FAILED;
                }
                else if (!(_class = GetClassFromEnvAndName()))
                {
                    _state = Base::Error;

                    LLBC_String errStr;
                    pyllbc_SetError(errStr.format("class name '%s' is not defined", _clsName.c_str()));

                    return LLBC_FAILED;
                }
            }
        }

        _state = Base::Done;
        _str.append(1, static_cast<char>(ch));

        return LLBC_OK;
    }

    static const LLBC_String legalNames(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.");
    if (legalNames.find(static_cast<char>(ch)) == LLBC_String::npos)
    {
        _state = Base::Error;
        pyllbc_SetError("invalid class name character: %c", ch);

        return LLBC_FAILED;
    }

    _clsName.append(1, static_cast<char>(ch));
    return LLBC_OK;
}

int pyllbc_PackLemma_Class::Process(Base *lemma)
{
    _state = Base::Error;
    pyllbc_SetError("class-lemma could not accept a lemma");

    return LLBC_FAILED;
}

PyObject *pyllbc_PackLemma_Class::Read(pyllbc_Stream *stream)
{
    if (UNLIKELY(_state != Base::Done))
    {
        pyllbc_SetError("class-lemma not done to unpack data");
        return nullptr;
    }
    else if (!_class)
    {
        pyllbc_SetError("not specific class to unpack data");
        return nullptr;
    }

    return stream->Read(_class);
}

int pyllbc_PackLemma_Class::Write(pyllbc_Stream *stream, PyObject *values)
{
    if (_state != Base::Done)
    {
        pyllbc_SetError("class-lemma not done for pack data");
        return LLBC_FAILED;
    }

    if (_class)
    {
        PyObject *valuesClass = PyObject_Type(values);
        if (_class != valuesClass)
        {
            Py_DECREF(valuesClass);

            LLBC_String errStr;
            pyllbc_SetError(errStr.format(
                "will pack values type not specific class: %s", _clsName.c_str()));

            return LLBC_FAILED;
        }

        Py_DECREF(valuesClass);
    }

    return stream->Write(values);
}

PyObject *pyllbc_PackLemma_Class::GetClassFromEnvAndName()
{
    std::vector<LLBC_String> subNames = _clsName.split(".");

    PyObject *nowObj = _compileEnv, *nestedObj;

    Py_INCREF(nowObj);
    const int subNameSize = static_cast<int>(subNames.size());
    for (int i = 0; i < subNameSize; ++i)
    {
        // First, we check now will parse object is Module type or not.
        // If is module type, get this module's dictionary.
        if (PyModule_Check(nowObj))
        {
            nestedObj = PyModule_GetDict(nowObj); // Borrowed reference.
            
            // Become nested object to new reference.
            Py_INCREF(nestedObj);
            // Dereference now object, and assign nested object into now object.
            Py_DECREF(nowObj);
            nowObj = nestedObj;
        }

        const LLBC_String &subName = subNames[i];
        if (PyDict_CheckExact(nowObj))
        {
            if (!(nestedObj = PyDict_GetItemString(nowObj, subName.c_str()))) // Borrowed reference.
            {
                Py_DECREF(nowObj);
                return nullptr;
            }

            // Become nested-class to new reference.
            Py_INCREF(nestedObj);
        }
        else
        {
            if (!(nestedObj = PyObject_GetAttrString(nowObj, subName.c_str()))) // New reference.
            {
                Py_DECREF(nowObj);
                return nullptr;
            }
        }

        Py_DECREF(nowObj);
        nowObj = nestedObj;
    }

    return nowObj;
}
