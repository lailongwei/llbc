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
        return NULL;

    LLBC_LoggerManager *loggerMgr = LLBC_LoggerManagerSingleton;
    if (loggerMgr->Initialize(cfgFile) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_LogMsg(PyObject *self, PyObject *args)
{
    int level;

    int line;
    char *msg;
    PyObject *fileObj;

    char *tag = NULL;
    char *loggerName = NULL;

    if (!PyArg_ParseTuple(args, "iOis|ss", &level, &fileObj, &line, &msg, &loggerName, &tag))
        return NULL;

    LLBC_LoggerManager *loggerMgr = LLBC_LoggerManagerSingleton;
    LLBC_Logger *logger = (!loggerName || LLBC_StrCmp(loggerName, "root") == 0) ? 
            loggerMgr->GetRootLogger() : loggerMgr->GetLogger(loggerName);
    if (!logger)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    char *file = NULL;
    if (PyObject_IsInstance(fileObj, PYLLBC_STR_CLS))
        if (!PyArg_Parse(fileObj, "s", &file))
            return NULL;

    int rtn;
    switch(level)
    {
    case LLBC_LogLevel::Debug:
        rtn = logger->Debug(tag, file, line, "%s", msg);
        break;

    case LLBC_LogLevel::Info:
        rtn = logger->Info(tag, file, line, "%s", msg);
        break;

    case LLBC_LogLevel::Warn:
        rtn = logger->Warn(tag, file, line, "%s", msg);
        break;

    case LLBC_LogLevel::Error:
        rtn = logger->Error(tag, file, line, "%s", msg);
        break;

    case LLBC_LogLevel::Fatal:
        rtn = logger->Fatal(tag, file, line, "%s", msg);
        break;

    default:
        pyllbc_SetError("Unknown log level");
        return NULL;
    }

    if (UNLIKELY(rtn != LLBC_OK))
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    Py_RETURN_NONE;
}
