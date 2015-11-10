/**
 * @file    ErrorHooker.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/03
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_ERROR_HOOKER_H__
#define __PYLLBC_COMM_ERROR_HOOKER_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The pyllbc error hooker class encapsulation.
 */
class LLBC_HIDDEN pyllbc_ErrorHooker
{
public:
    /**
     * Ctor & Dtor.
     */
    pyllbc_ErrorHooker();
    ~pyllbc_ErrorHooker();

public:
    /**
     * Install this hooker to pyllbc library.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Install();

    /**
     * Uninstall this hooker from pyllbc library.
     */
    void Uninstall();

    /**
     * Determine this hook already install to pyllbc library or not.
     * @return bool - the installed flag.
     */
    bool IsInstalled() const;

public:
    /**
     * Cleanup the hooker's hooked error informations.
     */
    void Cleanup();


    /**
     * Transfer hooked error to python.
     */
    void TransferHookedErrorToPython();

public:
    /**
     * When error cleared, will call this hooker.
     */
    void Hook_ErrClear();

    /**
     * When error set, will call this hooker.
     * @param[in] errDesc - the error describe.
     * @param[in] llbcErr - llbc library error no.
     * @param[in] errCls  - the error class(normal, not steal reference).
     * @param[in] tbObj   - the traceback object(normal, not steal reference).
     */
    void Hook_ErrSet(const LLBC_String &errDesc, int llbcErr, PyObject *errCls, PyObject *tbObj);

private:
    bool _installed;
    bool _transfering;

    int _llbcErrNo;
    LLBC_String _errDesc;
    PyObject *_errCls;
    PyObject *_tbObj;
};

#endif // !__PYLLBC_COMM_ERROR_HOOKER_H__
