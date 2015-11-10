# -*- coding: utf-8 -*-

import llbc as _llbc
from llbc import inl as _inl

class pyllbcPacketHeaderPartDesc(object):
    """
    The llbc library packet header part describe class encapsulation.
    Use to describe a packet header part.
    For example, if we want construct a length part in header, the code like follow:
        part_desc = llbc.PacketHeaderPartDesc()
        part_desc.part_len = 4  # Set part length is 4 bytes
        part_desc.is_len_part = True  # Set is length part
        part_desc.is_len_included_self = True  # Set length include this part
        ...
    """

    # The max part length define.
    MaxPartLen = _inl.PacketHeaderPartDesc_MaxPartLen

    def __init__(self):
        self._cobj = _inl.PacketHeaderPartDesc_New()

    def __del__(self):
        _inl.PacketHeaderPartDesc_Del(self._cobj)

    def is_valid(self):
        return _inl.PacketHeaderPartDesc_IsValid(self._cobj)

    @property
    def serial_no(self):
        return _inl.PacketHeaderPartDesc_GetSerialNo(self._cobj)
    @serial_no.setter
    def serial_no(self, value):
        _inl.PacketHeaderPartDesc_SetSerialNo(self._cobj, value)

    @property
    def part_len(self):
        return _inl.PacketHeaderPartDesc_GetPartLen(self._cobj)
    @part_len.setter
    def part_len(self, value):
        _inl.PacketHeaderPartDesc_SetPartLen(self._cobj, value)

    @property
    def is_len_part(self):
        return _inl.PacketHeaderPartDesc_IsLenPart(self._cobj)
    @is_len_part.setter
    def is_len_part(self, flag):
        _inl.PacketHeaderPartDesc_SetIsLenPart(self._cobj, flag)

    @property
    def is_len_included_self(self):
        return _inl.PacketHeaderPartDesc_IsLenIncludedSelf(self._cobj)
    @is_len_included_self.setter
    def is_len_included_self(self, flag):
        _inl.PacketHeaderPartDesc_SetIsLenIncludedSelf(self._cobj, flag)

    @property
    def is_opcode_part(self):
        return _inl.PacketHeaderPartDesc_IsOpcodePart(self._cobj)
    @is_opcode_part.setter
    def is_opcode_part(self, flag):
        return _inl.PacketHeaderPartDesc_SetIsOpcodePart(self._cobj, flag)

    @property
    def is_status_part(self):
        return _inl.PacketHeaderPartDesc_IsStatusPart(self._cobj)
    @is_status_part.setter
    def is_status_part(self, flag):
        return _inl.PacketHeaderPartDesc_SetIsStatusPart(self._cobj, flag)

    @property
    def is_serviceid_part(self):
        return _inl.PacketHeaderPartDesc_IsServiceIdPart(self._cobj)
    @is_serviceid_part.setter
    def is_serviceid_part(self, flag):
        return _inl.PacketHeaderPartDesc_SetIsServiceIdPart(self._cobj, flag)

    @property
    def is_flags_part(self):
        return _inl.PacketHeaderPartDesc_IsFlagsPart(self._cobj)
    @is_flags_part.setter
    def is_flags_part(self, flag):
        return _inl.PacketHeaderPartDesc_SetIsFlagsPart(self._cobj, flag)

    def __str__(self):
        return _inl.PacketHeaderPartDesc_ToString(self._cobj)

_llbc.PacketHeaderPartDesc = pyllbcPacketHeaderPartDesc

