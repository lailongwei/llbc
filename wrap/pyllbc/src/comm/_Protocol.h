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


LLBC_EXTERN_C PyObject *_pyllbc_layer2str(PyObject *self, PyObject *args)
{
    int layer;
    if (!PyArg_ParseTuple(args, "i", &layer))
        return nullptr;

    const LLBC_String &layerDesc = LLBC_ProtocolLayer::Layer2Str(layer);
    return PyString_FromStringAndSize(layerDesc.c_str(), layerDesc.length());
}

LLBC_EXTERN_C PyObject *_pyllbc_level2str(PyObject *self, PyObject *args)
{
    int level;
    if (!PyArg_ParseTuple(args, "i", &level))
        return nullptr;

    const LLBC_String &levelDesc = LLBC_ProtoReportLevel::Level2Str(level);
    return PyString_FromStringAndSize(levelDesc.c_str(), levelDesc.length());
}
