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

LLBC_EXTERN_C PyObject *_pyllbc_InitLoggerMgr(PyObject *self, PyObject *args)
{
    char *cfgFile;
    if (!PyArg_ParseTuple(args, "s", &cfgFile))
        return nullptr;

    LLBC_LoggerMgr *loggerMgr = LLBC_LoggerMgrSingleton;
    if (loggerMgr->Initialize(cfgFile) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return nullptr;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_UnInitLoggerMgr(PyObject *self, PyObject *args)
{
    LLBC_LoggerMgrSingleton->Finalize();

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_LogMsg(PyObject *self, PyObject *args)
{
    int level;

    int line;
    char *msg;
    PyObject *fileNameObj;
    PyObject *funcNameObj;

    char *tag = nullptr;
    char *loggerName = nullptr;

    if (!PyArg_ParseTuple(args, "iOiOs|ss", &level, &fileNameObj, &line, &funcNameObj, &msg, &loggerName, &tag))
        return nullptr;

    LLBC_LoggerMgr *loggerMgr = LLBC_LoggerMgrSingleton;
    LLBC_Logger *logger = (!loggerName || strcmp(loggerName, "root") == 0) ? 
            loggerMgr->GetRootLogger() : loggerMgr->GetLogger(loggerName);
    if (!logger)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return nullptr;
    }

    char *file = nullptr;
    if (PyObject_IsInstance(fileNameObj, PYLLBC_STR_CLS))
    {
        if (!PyArg_Parse(fileNameObj, "s", &file))
            return nullptr;
    }
    
    char *func = nullptr;
    if (PyObject_IsInstance(funcNameObj, PYLLBC_STR_CLS))
    {
        if (!PyArg_Parse(funcNameObj, "s", &func))
            return nullptr;
    }

    int rtn = logger->NonFormatOutput(level,
                                      tag,
                                      file,
                                      line,
                                      func,
                                      0,
                                      msg,
                                      -1);
    if (UNLIKELY(rtn != LLBC_OK))
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return nullptr;
    }

    Py_RETURN_NONE;
}
