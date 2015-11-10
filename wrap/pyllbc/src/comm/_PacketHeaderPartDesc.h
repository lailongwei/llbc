/**
 * @file    _PacketHeaderPartDesc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/01
 * @version 1.0
 *
 * @brief
 */

static inline PyObject *__GetDescFlag(PyObject *args, bool (LLBC_PacketHeaderPartDesc::*function)() const)
{
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    if ((desc->*function)())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static inline PyObject *__SetDescFlag(PyObject *args, LLBC_PacketHeaderPartDesc &(LLBC_PacketHeaderPartDesc::*function)(bool))
{
    PyObject *flag;
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "lO", &desc, &flag))
        return NULL;

    const int boolTestRet = PyObject_IsTrue(flag);
    if (UNLIKELY(boolTestRet == -1))
    {
        pyllbc_TransferPyError();
        return NULL;
    }

    (desc->*function)(boolTestRet ? true : false);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_New(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderPartDesc *desc = LLBC_New(LLBC_PacketHeaderPartDesc);
    return Py_BuildValue("l", desc);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_Del(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    LLBC_Delete(desc);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_GetSerialNo(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetSerialNo());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetSerialNo(PyObject *self, PyObject *args)
{
    int serialNo;
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "li", &desc, &serialNo))
        return NULL;

    desc->SetSerialNo(serialNo);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsLenPart(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsLenPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetIsLenPart(PyObject *self, PyObject *args)
{
    return __SetDescFlag(args, &LLBC_PacketHeaderPartDesc::SetIsLenPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsLenIncludedSelf(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsLenIncludedSelf);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetIsLenIncludedSelf(PyObject *self, PyObject *args)
{
    return __SetDescFlag(args, &LLBC_PacketHeaderPartDesc::SetIsLenIncludedSelf);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_GetPartLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetPartLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetPartLen(PyObject *self, PyObject *args)
{
    int partLen;
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "li", &desc, &partLen))
        return NULL;

    desc->SetPartLen(partLen);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsOpcodePart(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsOpcodePart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetIsOpcodePart(PyObject *self, PyObject *args)
{
    return __SetDescFlag(args, &LLBC_PacketHeaderPartDesc::SetIsOpcodePart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsStatusPart(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsStatusPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetIsStatusPart(PyObject *self, PyObject *args)
{
    return __SetDescFlag(args, &LLBC_PacketHeaderPartDesc::SetIsStatusPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsServiceIdPart(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsServiceIdPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetIsServiceIdPart(PyObject *self, PyObject *args)
{
    return __SetDescFlag(args, &LLBC_PacketHeaderPartDesc::SetIsServiceIdPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsFlagsPart(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsFlagsPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_SetIsFlagsPart(PyObject *self, PyObject *args)
{
    return __SetDescFlag(args, &LLBC_PacketHeaderPartDesc::SetIsFlagsPart);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_IsValid(PyObject *self, PyObject *args)
{
    return __GetDescFlag(args, &LLBC_PacketHeaderPartDesc::IsValid);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderPartDesc_ToString(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderPartDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    const LLBC_String repr = desc->ToString();
    return PyString_FromStringAndSize(repr.data(), repr.size());
}