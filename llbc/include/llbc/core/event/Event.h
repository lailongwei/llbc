/**
 * @file    Event.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/28
 * @version 1.0
 *
 * @biref
 */
#ifndef __LLBC_CORE_EVENT_EVENT_H__
#define __LLBC_CORE_EVENT_EVENT_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The event class encapsulation.
 */
class LLBC_EXPORT LLBC_Event
{
public:
    LLBC_Event(int id = 0);
    virtual ~LLBC_Event();

public:
    /**
     * Get the event Id.
     * @return int - the event Id.
     */
    int GetId() const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Event);

protected:
    int _id;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_EVENT_EVENT_H__
