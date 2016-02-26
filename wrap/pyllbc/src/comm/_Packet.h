/**
 * @file    _Packet.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/29
 * @version 1.0
 *
 * @brief
 */

#ifndef __PYLLBC_PACKET_INL_DEF
 #define __PYLLBC_PACKET_INL_DEF
#endif //! __PYLLBC_PACKET_INL_DEF

#ifdef __PYLLBC_PACKET_INL_DEF
 #define __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(valType, funcSuffix) \
    int serialNo;                                                    \
    LLBC_Packet *packet;                                             \
        if (!PyArg_ParseTuple(args, "li", &packet, &serialNo))       \
            return NULL;                                             \
                                                                     \
    valType val = packet->GetHeaderPartAs##funcSuffix(serialNo)      \

 #define __PYLLBC_PACKET_SET_HEADER_PART(valType, parseFmt) \
    valType val;                                            \
    int serialNo;                                           \
    LLBC_Packet *packet;                                    \
    if (!PyArg_ParseTuple(args, parseFmt, &packet, &serialNo, &val)) \
        return NULL;                                        \
                                                            \
    if (packet->SetHeaderPartVal(serialNo, val) != LLBC_OK) \
    {                                                       \
        LLBC_String fmtStr;                                 \
        pyllbc_TransferLLBCError(                           \
        __FILE__, __LINE__, LLBC_String().format(                            \
            "When set packet header part, partType: %s, serialNo: %d", #valType, serialNo)); \
                                                            \
        return NULL;                                        \
    }                                                       \
                                                            \
    Py_RETURN_NONE                                          \

#endif // __PYLLBC_PACKET_INL_DEF

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsSInt8(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(sint8, SInt8);
    return PyString_FromFormat("%c", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsUInt8(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(uint8, UInt8);
    return PyString_FromFormat("%u", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsSInt16(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(sint16, SInt16);
    return PyInt_FromLong(val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsUInt16(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(uint16, UInt16);
    return PyInt_FromLong(val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsSInt32(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(sint32, SInt32);
    return PyInt_FromLong(val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsUInt32(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(uint32, UInt32);
    return Py_BuildValue("I", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsSInt64(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(sint64, SInt64);
    return Py_BuildValue("L", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsUint64(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(uint64, UInt64);
    return Py_BuildValue("K", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsFloat(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(float, Float);
    return Py_BuildValue("f", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsDouble(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO(double, Double);
    return Py_BuildValue("d", val);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetHeaderPartAsStr(PyObject *self, PyObject *args)
{
    int serialNo;
    LLBC_Packet *packet;
    if (!PyArg_ParseTuple(args, "li", &packet, &serialNo))
        return NULL;

    const char *partData = reinterpret_cast<
        const char *>(packet->GetHeaderPartVal(serialNo));
    if (!partData)
        return PyString_FromStringAndSize("", 0);

    const size_t partLen = packet->GetHeaderPartLen(serialNo);

    return PyString_FromStringAndSize(partData, partLen);
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsSInt8(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(sint8, "lic");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsUInt8(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(uint8, "lii");
}
LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsSInt16(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(sint16, "lih");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsUInt16(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(uint16, "liH");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsSInt32(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(sint32, "lii");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsUInt32(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(uint32, "liI");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsSInt64(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(sint64, "liL");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsUInt64(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(uint64, "liK");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsFloat(PyObject *self ,PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(float, "lif");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsDouble(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(double, "lid");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsStr(PyObject *self, PyObject *args)
{
    __PYLLBC_PACKET_SET_HEADER_PART(LPCSTR, "lis");
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_SetHeaderPartAsBytes(PyObject *self, PyObject *args)
{
// 's#' parse format additional describe(copy from Python documentations):
//      Starting with Python 2.5 the type of the length argument can be controlled 
//      by defining the macro PY_SSIZE_T_CLEAN before including Python.h. If the macro 
//      is defined, length is a Py_ssize_t rather than an int.
#if !defined(PY_SSIZE_T_CLEAN)
    int valLen;
#else
    Py_ssize_t valLen;
#endif

    int serialNo;
    const char *val;
    LLBC_Packet *packet;
    if (!PyArg_ParseTuple(args, "lis#", &packet, &serialNo, &val, &valLen))
        return NULL;

    if (packet->SetHeaderPartVal(serialNo, val, valLen) != LLBC_OK)
    {
        LLBC_String addiErrMsg;
        addiErrMsg.format("When set packet header part, "
            "partType: %s, serialNo: %d", "Bytes", serialNo);
        pyllbc_TransferLLBCError(__FILE__, __LINE__, addiErrMsg);

        return NULL;
    }

    Py_RETURN_NONE;
}

#ifdef __PYLLBC_PACKET_INL_DEF
 #undef __PYLLBC_PACKET_GET_COBJ_AND_SERIAL_NO
 #undef __PYLLBC_PACKET_SET_HEADER_PART
 #undef __PYLLBC_PACKET_INL_DEF
#endif // __PYLLBC_PACKET_INL_DEF