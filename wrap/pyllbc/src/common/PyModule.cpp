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
#include "pyllbc/common/PyModule.h"

namespace
{
    typedef pyllbc_Module This;
}

pyllbc_Module::pyllbc_Module(const LLBC_String &name, This *parent)
: _name(name)
, _module(nullptr)
, _methods()

, _parent(nullptr)
, _subModules()

, _lazyAddObjs()

, _dict(nullptr)
{
    ASSERT(!_name.empty() &&
        "pyllbc_Module::ctor(): module name must be not null!");

    if (parent)
        parent->AddSubModule(this);
}

pyllbc_Module::~pyllbc_Module()
{
    /* Cleanup all lazy add objs. */
    for (_Objs::iterator it = _lazyAddObjs.begin();
         it != _lazyAddObjs.end();
         it++)
    {
        PyObject *obj = it->second;
        Py_DECREF(obj);
    }

    /* Destroy all sub modules. */
    LLBC_STLHelper::DeleteContainer(_subModules, false);

    /* Dereference self python module object. */
    Py_XDECREF(_module);
}

PyObject *pyllbc_Module::GetPyModule() const
{
    return _module;
}

PyObject *pyllbc_Module::GetModuleDict() const
{
    if (!_dict)
    {
        pyllbc_SetError("specific module not init, could not get module dict");
        return nullptr;
    }

    return _dict;
}

PyObject *pyllbc_Module::GetObject(PyObject *name)
{
    PyObject *dict = GetModuleDict();
    if (!dict)
        return nullptr;

    PyObject *obj = PyDict_GetItem(dict, name);
    if (!obj)
    {
        pyllbc_SetError("could not found module object");
        return nullptr;
    }

    return obj;
}

PyObject *pyllbc_Module::GetObject(const LLBC_String &name)
{
    PyObject *pyName = 
        PyString_FromStringAndSize(name.data(), name.size());

    PyObject *obj = GetObject(pyName);

    Py_DECREF(pyName);

    return obj;
}

const LLBC_String &pyllbc_Module::GetModuleName() const
{
    return _name;
}

This *pyllbc_Module::GetParentModule() const
{
    return _parent;
}

This *pyllbc_Module::GetSubModule(const LLBC_String &name) const
{
    std::vector<LLBC_String> names;
    LLBC_SplitString(name, ".", names);

    if (names.empty())
    {
        pyllbc_SetError("module name empty", LLBC_ERROR_INVALID);
        return nullptr;
    }
    
    _Modules::const_iterator it = _subModules.find(names[0]);
    if (it == _subModules.end())
    {
        pyllbc_SetError("not found sub module", LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    This *module = it->second;
    for (size_t i = 1; i < names.size(); ++i)
    {
        const LLBC_String &splittedName = names[i];
        if (splittedName.empty())
        {
            pyllbc_SetError("module name empty", LLBC_ERROR_INVALID);
            return nullptr;
        }

        if (!(module = module->GetSubModule(splittedName)))
            return nullptr;
    }

    return module;
}

int pyllbc_Module::AddSubModule(pyllbc_Module *module)
{
    if (module->GetParentModule())
    {
        pyllbc_SetError("will add sub module already exist parent module", LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }
    else if (GetSubModule(module->GetModuleName()))
    {
        pyllbc_SetError("sub module name repeat", LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    pyllbc_ClearError();

    module->_parent = this;
    const LLBC_String &moduleName = module->GetModuleName();
    _subModules.insert(std::make_pair(moduleName, module));

    return LLBC_OK;
}

int pyllbc_Module::AddMethod(const PyMethodDef &method)
{
    if (_module)
    {
        pyllbc_SetError(
            "module already build, not permit to add new method", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return _methods.AddMethod(method);
}

int pyllbc_Module::AddMethod(const LLBC_String &name, 
                             PyCFunction meth, 
                             int flags, 
                             const LLBC_String &doc)
{
    char *hName = LLBC_Malloc(char, name.size() + 1);
    memcpy(hName, name.c_str(), name.size());
    hName[name.size()] = '\0';

    char *hDoc = nullptr;
    if (!doc.empty())
    {
        hDoc = LLBC_Malloc(char, doc.size() + 1);
        memcpy(hDoc, doc.c_str(), doc.size());
        hDoc[doc.size()] = '\0';
    }

    PyMethodDef md;
    md.ml_name = hName;
    md.ml_meth = meth;
    md.ml_flags = flags;
    md.ml_doc = hDoc;

    return AddMethod(md);
}

int pyllbc_Module::AddObject(const LLBC_String &name, PyObject *obj)
{
    if (name.empty())
    {
        pyllbc_SetError("object name must be not empty", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_lazyAddObjs.find(name) != _lazyAddObjs.end())
    {
        pyllbc_SetError("object name repeat", LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    if (!_module)
    {
        _lazyAddObjs.insert(std::make_pair(name, obj));
        return LLBC_OK;
    }

    if (PyModule_AddObject(_module, name.c_str(), obj) != 0)
    {
        pyllbc_SetError(PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, bool obj)
{
    PyObject *o = PyBool_FromLong(obj ? 1 : 0);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, sint8 obj)
{
    PyObject *o = Py_BuildValue("c", obj);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, sint32 obj)
{
    PyObject *o = Py_BuildValue("i", obj);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, uint32 obj)
{
    PyObject *o = Py_BuildValue("I", obj);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, sint64 obj)
{
    PyObject *o = Py_BuildValue("L", obj);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, uint64 obj)
{
    PyObject *o = Py_BuildValue("K", obj);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, double obj)
{
    PyObject *o = Py_BuildValue("d", obj);
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::AddObject(const LLBC_String &name, const LLBC_String &obj)
{
    PyObject *o = Py_BuildValue("s", obj.c_str());
    if (AddObject(name, o) != LLBC_OK)
    {
        Py_DECREF(o);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Module::Build()
{
    if (!_module)
    {
        if (!(_module = Py_InitModule(_name.c_str(), _methods.GetMethods())))
        {
            pyllbc_SetError(PYLLBC_ERROR_COMMON);
            return LLBC_FAILED;
        }

        for (_Objs::iterator it = _lazyAddObjs.begin();
             it != _lazyAddObjs.end();
             )
        {
            PyObject *obj = it->second;
            const char *objName = it->first.c_str();
            if (PyModule_AddObject(_module, objName, obj) != 0)
            {
                pyllbc_SetError(PYLLBC_ERROR_COMMON);
                return LLBC_FAILED;
            }

            _lazyAddObjs.erase(it++);
        }
    }

    for (_Modules::iterator it = _subModules.begin();
         it != _subModules.end();
         it++)
    {
        This *subModule = it->second;
        if (subModule->Build() != LLBC_OK)
            return LLBC_FAILED;

        const LLBC_String &subModuleName = it->first;

        PyObject *modDict = PyModule_GetDict(_module);
        if (PyDict_GetItemString(modDict, subModuleName.c_str()))
            continue;

        PyObject *pySubModule = subModule->GetPyModule();

        Py_INCREF(pySubModule);
        if (AddObject(subModuleName, pySubModule) != LLBC_OK)
        {
            Py_DECREF(pySubModule);
            return LLBC_FAILED;
        }
    }

    _dict = PyModule_GetDict(_module);

    return LLBC_OK;
}
