# -*- coding: utf-8 -*-

import llbc as _llbc
from llbc import inl as _inl

class pyllbcPacketHeaderDesc(object):
    """
    The llbc library packet header describe class encapsulation.
    """

    # The max header length define.
    MaxHeaderLen = _inl.PacketHeaderDesc_MaxHeaderLen

    def __init__(self):
        self._cobj = _inl.PacketHeaderDesc_New()
        self._parts = {}

    def __del__(self):
        _inl.PacketHeaderDesc_Del(self._cobj)

    def add_part(self, part_desc):
        if not isinstance(part_desc, _llbc.PacketHeaderPartDesc):
            raise _llbc.error('part desc type error, type: {}'.format(type(part_desc)))

        _inl.PacketHeaderDesc_AddPart(self._cobj, part_desc._cobj)
        self._parts[part_desc.serial_no] = part_desc

        self._update_bookkeeping()

    def remove_part(self, serial_no):
        _inl.PacketHeaderDesc_RemovePart(self._cobj, serial_no)
        del self._parts[serial_no]

        self._update_bookkeeping()

    def get_part(self, serial_no):
        return self._parts.get(serial_no)

    def get_header_len(self):
        return _inl.PacketHeaderDesc_GetHeaderLen(self._cobj)

    def get_len_part_included_len(self):
        return _inl.PacketHeaderDesc_GetLenPartIncludedLen(self._cobj)

    def get_len_part_not_included_len(self):
        return _inl.PacketHeaderDesc_GetLenPartNotIncludedLen(self._cobj)

    def get_len_part_len(self):
        return _inl.PacketHeaderDesc_GetLenPartLen(self._cobj)

    def get_len_part_offset(self):
        return _inl.PacketHeaderDesc_GetLenPartOffset(self._cobj)

    def has_opcode_part(self):
        return _inl.PacketHeaderDesc_IsHasOpcodePart(self._cobj)

    def get_opcode_part_len(self):
        return _inl.PacketHeaderDesc_GetOpcodePartLen(self._cobj)

    def get_opcode_part_offset(self):
        return _inl.PacketHeaderDesc_GetOpcodePartOffset(self._cobj)

    def has_status_part(self):
        return _inl.PacketHeaderDesc_IsHasStatusPart(self._cobj)

    def get_status_part_len(self):
        return _inl.PacketHeaderDesc_GetStatusPartLen(self._cobj)

    def get_status_part_offset(self):
        return _inl.PacketHeaderDesc_GetStatusPartOffset(self._cobj)

    def has_serviceid_part(self):
        return _inl.PacketHeaderDesc_IsHasServiceIdPart(self._cobj)

    def get_serviceid_part_len(self):
        return _inl.PacketHeaderDesc_GetServiceIdPartLen(self._cobj)

    def get_serviceid_part_offset(self):
        return _inl.PacketHeaderDesc_GetServiceIdPartOffset(self._cobj)

    def has_flags_part(self):
        return _inl.PacketHeaderDesc_IsHasFlagsPart(self._cobj)

    def get_flags_part_len(self):
        return _inl.PacketHeaderDesc_GetFlagsPartLen(self._cobj)

    def get_flags_part_offset(self):
        return _inl.PacketHeaderDesc_GetFlagsPartOffset(self._cobj)

    def __str__(self):
        return _inl.PacketHeaderDesc_ToString(self._cobj)

    def _update_bookkeeping(self):
        pass

_llbc.PacketHeaderDesc = pyllbcPacketHeaderDesc

