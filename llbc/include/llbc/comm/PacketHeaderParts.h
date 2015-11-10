/**
 * @file    PacketHeaderParts.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/29
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PACKET_HEADER_PARTS_H__
#define __LLBC_COMM_PACKET_HEADER_PARTS_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN
/**
 * Pre-declare LLBC_Packet class.
 */
class LLBC_Packet;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The packet header parts class encapsulation.
 *        It provides a very simple way to set packet header parts.
 *        This class encapsulation is designed for very simplicity, high-preformence,
 *        so it not supports any GetXXX style methods.
 */
class LLBC_EXPORT LLBC_PacketHeaderParts
{
    typedef LLBC_PacketHeaderParts This;

public:
    LLBC_PacketHeaderParts();
    ~LLBC_PacketHeaderParts();

public:
    /**
     * Check has specific serial no's part or not.
     * @param[in] serialNo - the part serial no.
     * @return bool - return true if has specific serial no part, otherwise return -1.
     */
    bool IsHasPart(int serialNo) const;

    /**
     * Set header part value.
     * @param[in] serialNo - the serial number.
     * @param[in] val      - the part value.
     * @param[in] valSize  - the part value data size.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename T>
    int SetPart(int serialNo, const T &val);
    int SetPart(int serialNo, const void *val, size_t valSize);

    /**
     * Remove part.
     * @param[in] serialNo - part serial no.
     */
    void RemovePart(int serialNo);

public:
    /**
     * Set header parts to packet.
     * @param[in] packet - the packet.
     */
    void SetToPacket(LLBC_Packet &packet) const;

private:
    typedef struct
    {
        uint8 type;
        union {
            sint64 i64Val;
            LLBC_String *strVal;
            LLBC_MessageBlock *bytesVal;
        } value;
    } _Part;

    typedef std::map<int, _Part> _Parts;

private:

    static void SetPartToPacket(int serialNo, const _Part &part, LLBC_Packet &packet);

private:
    void SetConstructedPart(int serialNo, const _Part &part);
    static void CleanupPart(_Part &part);

private:
    _Parts _parts;
};

__LLBC_NS_END

#include "llbc/comm/PacketHeaderPartsImpl.h"

#endif // !__LLBC_COMM_PACKET_HEADER_PARTS_H__
