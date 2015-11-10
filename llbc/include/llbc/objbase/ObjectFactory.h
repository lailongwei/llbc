/**
 * @file    ObjectFactory.h
 * @author  Longwei lai<lailongwei@126.com>
 * @date    2014/02/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_OBJBASE_OBJECT_FACTORY_H__
#define __LLBC_OBJBASE_OBJECT_FACTORY_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

#include "llbc/objbase/Object.h"

__LLBC_NS_BEGIN

/**
 * \brief The object factory class encapsulation.
 *        In your project, can rewrite CreateObject() method to custom your object create way.
 */
class LLBC_EXPORT LLBC_ObjectFactory : public LLBC_Object
{
public:
    /**
     * Constructor/Destructor.
     */
    LLBC_ObjectFactory();
    virtual ~LLBC_ObjectFactory();

public:
    /**
     * Create the object.
     * @return LLBC_Object * - new object.
     */
    virtual LLBC_Object *CreateObject() const;

public:
    /**
     * Re-write clone method.
     * @return LLBC_Object * - the clone object.
     */
    virtual LLBC_Object *Clone() const;
};

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_OBJECT_FACTORY_H__
