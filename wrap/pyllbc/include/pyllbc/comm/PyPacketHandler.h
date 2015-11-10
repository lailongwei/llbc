/**
 * @file    PyPacketHandler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/28
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PYPACKET_HANDLER_H__
#define __PYLLBC_COMM_PYPACKET_HANDLER_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The python packet handler class encapsulation.
 */
class LLBC_HIDDEN pyllbc_PacketHandler
{
public:
    /**
     * Constructor & Destructor.
     * @param[in] opcode - the packet opcode.
     */
    pyllbc_PacketHandler(int opcode);
    ~pyllbc_PacketHandler();

public:
    /**
     * Set packet handler.
     * @param[in] handler - the packet handler, normal, not steal reference.
     *                      if handler is bound method, will hold im_self object too.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetHandler(PyObject *handler);

    /**
     * Handle packet.
     * handler search order(HSO):
     *      1> callable check(included function, bound method, callable object)
     *      2> search object has handle() method or not
     * @return PyObject * - the call returned PyObject object, if failed, return NULL.
     */
    PyObject *Handle(PyObject *packet);

public:
    /**
     * Get object string representation.
     * @return LLBC_String - the object representation.
     */
    LLBC_String ToString() const;

private:
    int _opcode;
    PyObject *_handler;
};

#endif // !__PYLLBC_COMM_PYPACKET_HANDLER_H__
