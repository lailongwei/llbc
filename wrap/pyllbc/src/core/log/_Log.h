/**
 * @file    _Log.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/15
 * @version 1.0
 *
 * @brief
 */

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
