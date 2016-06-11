/**
 * @file    Protocol.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PROTOCOL_LAYER_H__
#define __LLBC_COMM_PROTOCOL_LAYER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * \brief The protocol layers enumeartion.
 */
class LLBC_EXPORT LLBC_ProtocolLayer
{
public:
    enum
    {
        Begin,

        // Pack/Unpack layer, be responsible for raw bytestream and packet conversion.
        PackLayer = Begin,
        // Compress/DeCompress layer, be responsible for comp/decomp packet.
        CompressLayer,
        // Encode/Decode layer, be responsible for encode/decode packet.
        CodecLayer,

        End
    };

public:
    /**
     * Check given protocol layer legal or not.
     * @param[in] layer - the protocol layer.
     * @return bool - return true if validate, otherwise return -1.
     */
    static bool IsValid(int layer);

    /**
     * Get the protocol layer string representation.
     * @param[in] layer - protocol layer.
     * @return const LLBC_String & - the protocol layer string representation.
     */
    static const LLBC_String &Layer2Str(int layer);
};

__LLBC_NS_END

#endif // !__LLBC_COMM_PROTOCOL_LAYER_H__
