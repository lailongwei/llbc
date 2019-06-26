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

#include "pyllbc/common/Common.h"

LLBC_EXTERN_C PyObject *_pyllbc_Property_New(PyObject *self, PyObject *args)
{
    char *file;
    if (!PyArg_ParseTuple(args, "s", &file))
        return NULL;

    LLBC_Property *prop = LLBC_New0(LLBC_Property);
    if (LLBC_StrLenA(file) == 0)
        return Py_BuildValue("l", prop);

    if (prop->LoadFromFile(file) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, prop->GetLoadErrorDesc());
        LLBC_Delete(prop);

        return NULL;
    }

    return Py_BuildValue("l", prop);
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_Delete(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "l", &prop))
        return NULL;

    LLBC_Delete(prop);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_FromContent(PyObject *self, PyObject *args)
{
    const char *content;
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "ls", &prop, &content))
        return NULL;

    if (prop->LoadFromContent(content) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, prop->GetLoadErrorDesc());
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_FromFile(PyObject *self, PyObject *args)
{
    const char *file;
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "ls", &prop, &file))
        return NULL;

    if (prop->LoadFromFile(file) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, prop->GetLoadErrorDesc());
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_ToContent(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "l", &prop))
        return NULL;

    LLBC_String content;
    if (prop->SaveToContent(content) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    return PyString_FromStringAndSize(content.data(), content.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_ToFile(PyObject *self, PyObject *args)
{
    const char *file;
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "ls", &prop, &file))
        return NULL;

    if (prop->SaveToFile(file) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_GetProperty(PyObject *self, PyObject *args)
{
    const char *name;
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "ls", &prop, &name))
        return NULL;

    const LLBC_Property *subProp = prop->GetProperty(name);
    if (!subProp)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, 
            LLBC_String().format("When get property, name: %s", name));
        return NULL;
    }

    // Get python layer's Property class object.
    PyObject *propCls = pyllbc_TopModule->GetObject("Property"); // Borrowed reference.
    if (UNLIKELY(!propCls))
        return NULL;

    // Create python layer's Property instance object.
    PyObject *pySubProp = PyObject_CallObject(propCls, NULL); // New reference.
    if (UNLIKELY(!pySubProp))
    {
        pyllbc_TransferPyError();
        return NULL;
    }

    // Get the new old sub property.
    pyllbc_ObjAttrOptr attrOptr(pySubProp);

    long oldSubProp = 0;
    if (attrOptr.GetAttr("_c_obj", oldSubProp) != LLBC_OK)
    {
        Py_DECREF(pySubProp);
        return NULL;
    }

    // Set new sub property.
    LLBC_Property *copySubProp = LLBC_New1(LLBC_Property, *subProp);
    if (attrOptr.SetAttr("_c_obj", reinterpret_cast<long>(copySubProp)) != LLBC_OK)
    {
        Py_DECREF(pySubProp);
        LLBC_Delete(copySubProp);

        return NULL;
    }
    
    // Succeed, delete old sub property.
    LLBC_Delete(reinterpret_cast<LLBC_Property *>(oldSubProp));

    return pySubProp;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_GetPropertyCount(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "l", &prop))
        return NULL;

    return PyInt_FromSize_t(prop->GetPropertyCount());
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_GetPropertyNames(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    PyObject *nest = NULL;
    if (!PyArg_ParseTuple(args, "l|O", &prop, &nest))
        return NULL;

    bool cNest = false;
    if (nest)
    {
        const int checkRet = PyObject_IsTrue(nest);
        if (UNLIKELY(checkRet == -1))
        {
            pyllbc_TransferPyError();
            return NULL;
        }

        cNest = !!checkRet;
    }

    const LLBC_Strings names = prop->GetPropertyNames(cNest);

    PyObject *tuple = PyTuple_New(static_cast<Py_ssize_t>(names.size()));
    if (UNLIKELY(!tuple))
    {
        pyllbc_TransferPyError();
        return NULL;
    }

    for (LLBC_Strings::size_type i = 0; i < names.size(); ++i)
        PyTuple_SET_ITEM(tuple, i, 
            PyString_FromStringAndSize(names[i].data(), names[i].size()));

    return tuple;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_GetValue(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    const char *name = NULL;
    if (!PyArg_ParseTuple(args, "l|s", &prop, &name))
        return NULL;

    name = name ? name : "";
    const LLBC_Variant value = prop->GetValue(name);

	const int lastErrNo = LLBC_GetLastError();
    if (lastErrNo != LLBC_OK)
    {
		if (lastErrNo != LLBC_ERROR_NOT_FOUND)
		{
			pyllbc_TransferLLBCError(
				__FILE__, __LINE__, LLBC_String().format("When get property value, name: %s", name));
			return NULL;
		}

        Py_RETURN_NONE;
    }

    LLBC_String valueStr;
    if (!value.IsNil())
        valueStr = value.ValueToString();

    return PyString_FromStringAndSize(valueStr.data(), valueStr.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_SetValue(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    const char *name;
    PyObject *value = NULL, *comments = NULL;
    if (!PyArg_ParseTuple(args, "l|sOO", &prop, &name, &value, &comments))
        return NULL;

    LLBC_String valueStr;
    if (value)
    {
        if (!pyllbc_TypeDetector::IsNone(value))
        {
            valueStr = pyllbc_ObjUtil::GetObjStr(value);
            if (PyErr_Occurred())
                return NULL;
        }
    }

    LLBC_String commentsStr;
    if (comments && !pyllbc_TypeDetector::IsNone(comments))
    {
        commentsStr = pyllbc_ObjUtil::GetObjStr(comments);
        if (PyErr_Occurred())
            return NULL;
    }

    if (prop->SetValue(name, valueStr, commentsStr) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When set property value");
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_GetComments(PyObject *self, PyObject *args)
{
    const char *name;
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "ls", &prop, &name))
        return NULL;

    LLBC_String comments = prop->GetComments(name);
    LLBC_SetLastError(LLBC_ERROR_SUCCESS); //! Ignore this error.

    return PyString_FromStringAndSize(comments.data(), comments.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_SetComments(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    const char *name, *comments;
    if (!PyArg_ParseTuple(args, "lss", &prop, &name, &comments))
        return NULL;

    if (prop->SetComments(name, comments) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_HasProperty(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    const char *name;
    if (!PyArg_ParseTuple(args, "ls", &prop, &name))
        return NULL;

    const bool hasProp = prop->HasProperty(name);
    PyObject *pyHasProp = hasProp ? Py_True : Py_False;

    Py_INCREF(pyHasProp);
    return pyHasProp;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_RemoveProperty(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    const char *name;
    PyObject *removeAll;
    if (!PyArg_ParseTuple(args, "ls|O", &prop, &name, &removeAll))
        return NULL;

    bool cRemoveAll = false;
    if (removeAll)
    {
        const int checkRet = PyObject_IsTrue(removeAll);
        if (UNLIKELY(checkRet == -1))
        {
            pyllbc_TransferPyError("When remove property");
            return NULL;
        }

        cRemoveAll = !!checkRet;
    }

    if (prop->RemoveProperty(name, cRemoveAll) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When remove property");
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Property_RemoveAllPropertyes(PyObject *self, PyObject *args)
{
    LLBC_Property *prop;
    if (!PyArg_ParseTuple(args, "l", &prop))
        return NULL;

    prop->RemoveAllProperties();

    Py_RETURN_NONE;
}
