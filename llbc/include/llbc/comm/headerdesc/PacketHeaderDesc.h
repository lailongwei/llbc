/**
 * @file    PacketHeaderDesc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/29
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PACKET_HEADER_DESC_H__
#define __LLBC_COMM_PACKET_HEADER_DESC_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/headerdesc/PacketHeaderPartDesc.h"
#include "llbc/comm/headerdesc/IPacketHeaderDescFactory.h"

__LLBC_NS_BEGIN

/**
 * \brief The packet header describe.
 */
class LLBC_EXPORT LLBC_PacketHeaderDesc
{
    typedef LLBC_PacketHeaderDesc This;

public:
    /**
     * The max header length define, header length should not greater than or equal than this value.
     */
    static const size_t MaxHeaderLen = 0x7fff;

public:
    /**
     * Constructor(s) & Destructor.
     */
    LLBC_PacketHeaderDesc();
    LLBC_PacketHeaderDesc(const This &another);
    virtual ~LLBC_PacketHeaderDesc();

public:
    /**
     * Add header part describe.
     * This method is chain call method, use like follow:
     * headerDesc->AddPartDesc()
     *             .SetSerialNo(1)
     *             .SetPartLen(8)
     *             ... ...
     *             .Done();
     * @return LLBC_PacketHeaderPartDesc - the will be add part describe.
     */
    LLBC_PacketHeaderPartDesc AddPartDesc();

    /**
     * Add header part describe.
     * @param[in] partDes - the part describe.
     */
    int AddPartDesc(const LLBC_PacketHeaderPartDesc &partDesc);

    /**
     * Remove specific serial No part describe.
     * @param[in] serialNo - the serial No.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemovePartDesc(int serialNo);

    /**
     * Get the packet length.
     * @return size_t - the header length.
     */
    size_t GetHeaderLen() const;

    /**
     * Get the length part included length.
     * @return size_t - the length part included length.
     */
    size_t GetLenPartIncludedLen() const;

    /**
     * Get the length part not included length.
     * @return size_t - the length part not included length.
     */
    size_t GetLenPartNotIncludedLen() const;

public:
    const LLBC_PacketHeaderPartDesc *GetPart(int serialNo) const;

    /**
     * Get specific serial No part's offset.
     * @param[in] serialNo - the part serial No.
     * @return size_t - if success, return the part offset, otherwise, return This::MaxHeaderLen.
     */
    size_t GetPartOffset(int serialNo) const;

    /**
     * Get specific serial No part's length.
     * @param[in] serialNo - the part serial No.
     * @return size_t - if success, return the part length, otherwise, return LLBC_PacketHeaderPartDesc::MaxPartLen.
     */
    size_t GetPartLen(int serialNo) const;

public:
    /**
     * Get the length part describe.
     * @return const LLBC_PacketHeaderPartDesc * - the length part describe.
     */
    const LLBC_PacketHeaderPartDesc *GetLenPart() const;

    /**
     * Get the length part offset.
     * @return size_t - if success return the length part offset, otherwise return This::MaxHeaderLen.
     */
    size_t GetLenPartOffset() const;
    /**
     * Get the length part length.
     * @return size_t - if success, return the length part length, otherwise return LLBC_PacketHeaderPartDesc::MaxPartLen.
     */
    size_t GetLenPartLen() const;

    /**
     * Check the header has opcode part or not.
     * @return bool - the opcode part flag.
     */
    bool IsHasOpcodePart() const;
    /**
     * Get the opcode part offset.
     * @return size_t - if success return the opcode part offset, otherwise return This::MaxHeaderLen.
     */
    size_t GetOpcodePartOffset() const;
    /**
     * Get the opcode part length.
     * @return size_t - if success, return the opcode part length, otherwise return LLBC_PacketHeaderPartDesc::MaxPartLen.
     */
    size_t GetOpcodePartLen() const;

    /**
     * Check the header has status part or not.
     * @return bool - the status part flag.
     */
    bool IsHasStatusPart() const;
    /**
     * Get the status part offset.
     * @return size_t - if success return the status part offset, otherwise return This::MaxHeaderLen.
     */
    size_t GetStatusPartOffset() const;
    /**
     * Get the status part length.
     * @return size_t - if success, return the status part length, otherwise return LLBC_PacketHeaderPartDesc::MaxPartLen.
     */
    size_t GetStatusPartLen() const;

    /**
     * Check the header has serviceId part or not.
     * @return bool - the serviceId part flag.
     */
    bool IsHasServiceIdPart() const;
    /**
     * Get the serviceId part offset.
     * @return size_t - if success return the serviceId part offset, otherwise return This::MaxHeaderLen.
     */
    size_t GetServiceIdPartOffset() const;
    /**
     * Get the serviceId part length.
     * @return size_t - if success, return the serviceId part length, otherwise return LLBC_PacketHeaderPartDesc::MaxPartLen.
     */
    size_t GetServiceIdPartLen() const;

    /**
     * Check the header has flags part or not.
     * @return bool - the flags part flag.
     */
    bool IsHasFlagsPart() const;
    /**
     * Get the flags part offset.
     * @return size_t - if success return the flags part offset, otherwise return This::MaxHeaderLen.
     */
    size_t GetFlagsPartOffset() const;
    /**
     * Get the flags part length.
     * @return size_t - if success, return the flags part length, otherwise return LLBC_PacketHeaderPartDesc::MaxPartLen.
     */
    size_t GetFlagsPartLen() const;

public:
    /**
     * Get the header desc string representation.
     * @return LLBC_String - the header describe string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * operation =.
     */
    This &operator =(const This &another);

private:
    /**
     * Check the new part describe object can add to the header describe object or not.
     */
    int IsCanAdd(const LLBC_PacketHeaderPartDesc &partDesc) const;

    /**
     * Update the extra bookkeeping.
     */
    void UpdateExtraBookKeeping();

private:
    typedef std::map<int, LLBC_PacketHeaderPartDesc> _Parts;
    _Parts _parts;

private:
    typedef std::map<int, size_t> _PartOffsets;
    _PartOffsets _partOffsets;

    size_t _headerLen;
    size_t _lenPartIncludedLen;

private:
    size_t _lenPartOffset;
    const LLBC_PacketHeaderPartDesc *_lenPart;

    size_t _opcodePartOffset;
    const LLBC_PacketHeaderPartDesc *_opcodePart;

    size_t _statusPartOffset;
    const LLBC_PacketHeaderPartDesc *_statusPart;

    size_t _serviceIdPartOffset;
    const LLBC_PacketHeaderPartDesc *_serviceIdPart;

    size_t _flagsPartOffset;
    const LLBC_PacketHeaderPartDesc *_flagsPart;
};

__LLBC_NS_END

#include "llbc/comm/headerdesc/PacketHeaderDescImpl.h"

#endif // !__LLBC_COMM_PACKET_HEADER_DESC_H__

