/**
 * @file    _Protocol.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/16
 * @version 1.0
 *
 * @biref
 */

LLBC_EXTERN_C PyObject *_pyllbc_layer2str(PyObject *self, PyObject *args)
{
    int layer;
    if (!PyArg_ParseTuple(args, "i", &layer))
        return NULL;

    const LLBC_String &layerDesc = LLBC_ProtocolLayer::Layer2Str(layer);
    return PyString_FromStringAndSize(layerDesc.c_str(), layerDesc.length());
}

LLBC_EXTERN_C PyObject *_pyllbc_level2str(PyObject *self, PyObject *args)
{
    int level;
    if (!PyArg_ParseTuple(args, "i", &level))
        return NULL;

    const LLBC_String &levelDesc = LLBC_ProtoReportLevel::Level2Str(level);
    return PyString_FromStringAndSize(levelDesc.c_str(), levelDesc.length());
}
