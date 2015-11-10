/**
 * @file    PyEvent.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/09/06
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PY_EVENT_H__
#define __PYLLBC_COMM_PY_EVENT_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The python event c++ wrap class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Event : public LLBC_Event
{
public:
    /**
     * Constructor.
     * @param[in] id   - the event Id.
     * @param[in] data - the event data, normal, not steal reference to data.
     */
    pyllbc_Event(int id, PyObject *data);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Event();

public:
    /**
     * Get event data(new reference).
     * @return PyObject * - the event data.
     */
    PyObject *GetData();

private:
    PyObject *_data;
};

#endif // !__PYLLBC_COMM_PY_EVENT_H__
