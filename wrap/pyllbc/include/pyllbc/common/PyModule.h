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
#include "pyllbc/common/MethodTable.h"

class pyllbc_Module
{
    typedef pyllbc_Module This;

public:
    /**
     * Parameter constructor.
     * @param[in] name   - the module name.
     * @param[in] parent - the parent module, default is null.
     */
    pyllbc_Module(const LLBC_String &name, This *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Module();

public:
    /**
     * Get python module object.
     * @return PyObject * - the module object(borrowed reference).
     */
    PyObject *GetPyModule() const;

    /**
     * Get the dictionary object that implements module's namespace.
     * this object is the same as the __dict__ attribute of the module
     * object.
     * @return PyObject * - the python dictionary object(borrowed reference).
     */
    PyObject *GetModuleDict() const;

    /**
     * Get module object.
     * @param[in] name - the object name(object/string).
     * @return PyObject * - the module object, return nullptr if fail(borrowed reference).
     */
    PyObject *GetObject(PyObject *name);
    PyObject *GetObject(const LLBC_String &name);

    /**
     * Get the module name.
     * @return const LLBC_String & - the module name.
     */
    const LLBC_String &GetModuleName() const;

    /**
     * Get parent module.
     * @return This * - the parent module pointer.
     */
    This *GetParentModule() const;

    /**
     * Get sub module.
     * @param[in] name - the module name, which can use '.' character to split multi module names.
     * @return This * - the sub module pointer.
     */
    This *GetSubModule(const LLBC_String &name) const;

    /**
     * Add sub module to this module.
     * @param[in] module - the sub module.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddSubModule(pyllbc_Module *module);

    /**
     * Add python method.
     * @param[in] method - python method.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddMethod(const PyMethodDef &method);

    /**
     * Add python method.
     * @param[in] name  - python method name.
     * @Param[in] meth  - method function.
     * @param[in] flags - method flags.
     * @param[in] doc   - the method document.
     * return int - return 0 if success, otherwise return -1.
     */
    int AddMethod(const LLBC_String &name,
                  PyCFunction meth,
                  int flags,
                  const LLBC_String &doc = "");

    /**
     * Add python object to module.
     * @param[in] name - the object name.
     * @param[in] obj  - the python object, it steals a references to store.
     * @return int - return 0 if success, otherwise return false.
     */
    int AddObject(const LLBC_String &name, PyObject *obj);

    /**
     * Add python object from c/c++ raw type.
     * @param[in] name - the object name.
     * @param[in] obj  - the obj value.
     * @return int - return 0 if success, otherwise return false.
     */
    int AddObject(const LLBC_String &name, bool obj);
    int AddObject(const LLBC_String &name, sint8 obj);
    int AddObject(const LLBC_String &name, sint32 obj);
    int AddObject(const LLBC_String &name, uint32 obj);
    int AddObject(const LLBC_String &name, sint64 obj);
    int AddObject(const LLBC_String &name, uint64 obj);
    int AddObject(const LLBC_String &name, double obj);
    int AddObject(const LLBC_String &name, const LLBC_String &obj);

public:
    /**
     * Buildup module.
     * @return int - return 0 if success, otherwise return false.
     */
    int Build();

private:
    LLBC_String _name;
    PyObject *_module;
    pyllbc_MethodTable _methods;

    typedef std::map<LLBC_String, pyllbc_Module *> _Modules;
    This *_parent;
    _Modules _subModules;

    typedef std::map<LLBC_String, PyObject *> _Objs;
    _Objs _lazyAddObjs;

    PyObject *_dict;
};

#include "pyllbc/common/TopModule.h"
#include "pyllbc/common/InlModule.h"


