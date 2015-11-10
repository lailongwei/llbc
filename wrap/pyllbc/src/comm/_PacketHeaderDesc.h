/**
 * @file    _PacketHeaderDesc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/01
 * @version 1.0
 *
 * @brief
 */

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_New(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc = LLBC_New(LLBC_PacketHeaderDesc);
    return Py_BuildValue("l", desc);
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_Del(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    LLBC_Delete(desc);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_AddPart(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *headerDesc;
    LLBC_PacketHeaderPartDesc *partDesc;
    if (!PyArg_ParseTuple(args, "ll", &headerDesc, &partDesc))
        return NULL;

    if (headerDesc->AddPartDesc(*partDesc) != LLBC_RTN_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When add part desc to header desc");
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_RemovePart(PyObject *self, PyObject *args)
{
    int serialNo;
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "li", &desc, &serialNo))
        return NULL;

    if (desc->RemovePartDesc(serialNo) != LLBC_RTN_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When remove part desc from header desc");
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetHeaderLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetHeaderLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetLenPartIncludedLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetLenPartIncludedLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetLenPartNotIncludedLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetLenPartNotIncludedLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetLenPartLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetLenPartLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetLenPartOffset(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetLenPartOffset());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_IsHasOpcodePart(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;
    
    if (desc->IsHasOpcodePart())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetOpcodePartLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetOpcodePartLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetOpcodePartOffset(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetOpcodePartOffset());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_IsHasStatusPart(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;
    
    if (desc->IsHasStatusPart())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetStatusPartLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetStatusPartLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetStatusPartOffset(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetStatusPartOffset());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_IsHasServiceIdPart(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;
    
    if (desc->IsHasServiceIdPart())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetServiceIdPartLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetServiceIdPartLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetServiceIdPartOffset(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetServiceIdPartOffset());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_IsHasFlagsPart(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;
    
    if (desc->IsHasFlagsPart())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetFlagsPartLen(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetFlagsPartLen());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_GetFlagsPartOffset(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    return Py_BuildValue("i", desc->GetFlagsPartOffset());
}

LLBC_EXTERN_C PyObject *_pyllbc_PacketHeaderDesc_ToString(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    const LLBC_String repr = desc->ToString();
    return PyString_FromStringAndSize(repr.data(), repr.size());
}