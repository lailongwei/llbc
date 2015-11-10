/**
 * @file    PacketHeaderPartDesc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/28
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PACKET_HEADER_PART_DESC_H__
#define __LLBC_COMM_PACKET_HEADER_PART_DESC_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare LLBC_PacketHeaderDesc class.
 */
class LLBC_PacketHeaderDesc;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The LLBC library packet header part describe class encapsulation.
 */
class LLBC_EXPORT LLBC_PacketHeaderPartDesc
{
    typedef LLBC_PacketHeaderPartDesc This;

public:
    /**
     * The max part len define, any part should not greater than or equal than this value.
     */
    static const size_t MaxPartLen = 0x7fff;

public:
    /**
     * Constructors & Destructor.
     */
    LLBC_PacketHeaderPartDesc(LLBC_PacketHeaderDesc *headerDesc = NULL);
    LLBC_PacketHeaderPartDesc(const This &another);

    virtual ~LLBC_PacketHeaderPartDesc();

public:
    /**
     * Get the header part serial No.
     * @return int - the header part serial No.
     */
    int GetSerialNo() const;
    /**
     * Set the header part serial No.
     * @param[in] serialNo - the serial No.
     * @return This & - the this object reference.
     */
    This &SetSerialNo(int serialNo);

public:
    /**
     * Check this header part is length part or not.
     * @return bool - return true means this header part is length part, otherwise return false.
     */
    bool IsLenPart() const;
    /**
     * Set the header part's length part flag.
     * @param[in] lenPart - the length part flag.
     * @return This & - the this object reference.
     */
    This &SetIsLenPart(bool lenPart);

    /**
     * Check len header part length included self length or not.
     * @return bool - return true means len header part length included self length, otherwise return false.
     */
    bool IsLenIncludedSelf() const;
    /**
     * Set the len header part length included self length flag.
     * @param[in] includedSelf - the included self length flag.
     * @return This & - the this object reference.
     */
    This &SetIsLenIncludedSelf(bool includedSelf);

    /**
     * Get the part length.
     * @return size_t - the part length.
     */
    size_t GetPartLen() const;
    /**
     * Set the part length.
     * @param[in] len - the part length.
     * @return This & - the this object reference.
     */
    This &SetPartLen(size_t len);

public:
    /**
     * Check the part is opcode part or not.
     * @return bool - the opcode part flag.
     */
    bool IsOpcodePart() const;
    /**
     * Mark this part is opcode part.
     * @param[in] opcodePart - the opcode part flag.
     * @return This & - the this object reference.
     */
    This &SetIsOpcodePart(bool opcodePart);

    /**
     * Check this part is status part or not.
     * @return bool - the status part flag.
     */
    bool IsStatusPart() const;
    /**
     * Mark this part is status part.
     * @param[in] statusPart - the status part flag.
     * @return This & - the this object reference.
     */
    This &SetIsStatusPart(bool statusPart);

    /**
     * Check this part is serviceId part or not.
     * @return bool - the serviceId part flag.
     */
    bool IsServiceIdPart() const;
    /**
     * Mark this part is serviceId part.
     * @param[in] serviceIdPart - the serviceId part flag.
     * @return This & - the this object reference.
     */
    This &SetIsServiceIdPart(bool serviceIdPart);

    /**
     * Check this part is flags part or not.
     * @return bool - the flags part flag.
     */
    bool IsFlagsPart() const;
    /**
     * Mark this part is flags part.
     * @param[in] flagsPart - the flags part flag.
     * @return This & - the this object reference.
     */
    This &SetIsFlagsPart(bool flagsPart);

public:
    /** 
     * Check the part has fixed value or not.
     * @return bool - the fixed value flag.
     */
    bool IsHasFixedValue() const;

    /**
     * Get the part's fixed value.
     * @return [const] void * - the fixed value, if not has fixed value, return NULL.
     */
    void *GetFixedValue();
    const void *GetFixedValue() const;

    /**
     * Set the part's fixed value.
     * @param[in] value - the will set fixed value pointer.
     * @return This & - the this object reference, if successfully, 
     *                  LLBC_GetLastError() return 0, otherwise return non 0.
     */
    This &SetFixedValue(const void *value);

    /**
     * Clear the part's fixed value.
     * @return This & - the this object reference.
     */
    This &ClearFixedValue();

public:
    /**
     * Check this header part's validity.
     * @return bool - return true if this header part valid, otherwise return false.
     */
    bool IsValid() const;

    /**
     * Done this part describe, used by LLBC_PacketHeaderDesc::Add() method.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Done();

public:
    /**
     * Get the header part describe string representation.
     * @return LLBC_String representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * The '=' operation function.
     */
    This &operator =(const This &another);

    /**
     * The '<' operation function.
     */
    bool operator <(const This &another) const;

private:
    LLBC_PacketHeaderDesc *_headerDesc;

private:
    bool _hasBeenSet;
    
    int _serialNo;

    bool _lenPart;
    bool _lenIncludedSelf;

    bool _opcodePart;
    bool _statusPart;
    bool _serviceIdPart;
    bool _flagsPart;

    size_t _partLen;

private:
    void *_fixedValue;
};

__LLBC_NS_END

#include "llbc/comm/headerdesc/PacketHeaderPartDescImpl.h"

#endif // !__LLBC_COMM_PACKET_HEADER_PART_DESC_H__
