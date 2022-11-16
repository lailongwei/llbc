# -*- coding: utf-8 -*-

import inspect

import llbc

class pyllbcStream(object):
    """
    Stream class encapsulation, use to pack/unpack data sequence.
    """
    def __init__(self, cap=0, init_obj=None, endian=llbc.Endian.MachineEndian):
        self.__c_obj = llbc.inl.NewPyStream(self, cap, endian)
        self.packobj(init_obj)

    def __del__(self):
        llbc.inl.DelPyStream(self.__c_obj)

    @property
    def endian(self):
        """
        Get stream endian setting(see llbc.Endian module).
        """
        return llbc.inl.GetPyStreamEndian(self.__c_obj)

    @endian.setter
    def endian(self, e):
        """
        Set stream endian(see llbc.Endian module).
        """
        llbc.inl.SetPyStreamEndian(self.__c_obj, e)

    @property
    def pos(self):
        """
        Get stream current reading/writing position.
        """
        return llbc.inl.GetPyStreamPos(self.__c_obj)

    @pos.setter
    def pos(self, p):
        """
        Set stream current reading/writing position.
        """
        llbc.inl.SetPyStreamPos(self.__c_obj, p)

    @property
    def cap(self):
        """
        Get stream capacity(unsafe method, capacity will automatic adjust by stream).
        """
        return llbc.inl.GetPyStreamCap(self.__c_obj)

    @cap.setter
    def cap(self, s):
        """
        Set stream capacity(unsafe method, capacity will automatic adjust by stream).
        """
        llbc.inl.SetPyStreamCap(self.__c_obj, s)

    @property
    def raw(self):
        """
        Get stream memery view as buffer.
        """
        return llbc.inl.PyStreamGetRaw(self.__c_obj)

    @raw.setter
    def raw(self, r):
        """
        Set stream raw memory from str/buffer/bytearray.
        """
        llbc.inl.PyStreamSetRaw(self.__c_obj, r)

    @property
    def cobj(self):
        """
        Get raw pyllbc stream object(calling by c/c++ layer).
        """
        return self.__c_obj

    def __str__(self):
        """
        Get human readable stream data's string representation.
        """
        import binascii
        return binascii.hexlify(self.raw)

    @staticmethod
    def getcachedsize():
        return llbc.inl.PyStreamGetCachedSize()

    @staticmethod
    def getcachelimit():
        return llbc.inl.PyStreamGetCacheLimit()

    @staticmethod
    def setcachelimit(lmt):
        llbc.inl.PyStreamSetCacheLimit(lmt)

    @staticmethod
    def discardexpr(expr):
        llbc.inl.PyStreamDiscardExpr(expr)

    @staticmethod
    def discardallexprs():
        llbc.inl.PyStreamDiscardAllExprs()

    def unpack(self, fmt):
        """
        Unpack data according to the given format. the result is a tuple even if it contents exactly one item.
        format strings:
           c: char value(like b).
           b: byte value(like c).

           B: boolean value.
           s: short value.
           i: integer value.
           q: signed long long value.
           f: float value.
           d: double value(only support Fomat method).

           S: string value(null terminated).
           S#: string value, use another pack/unpack algorithm, 4 bytes length + string content(not include NULL character).
           U: unicode value.

           A: byte array value.
           F: buffer value.

           N: None value.

           C: class type, will automatic call class.encode() method to decode must tell stream this class name, 
              use C<ClassName> semantic.
           (): tuple type, if only has one element,  it represent tuple all element type is the given type, otherwise
               the tuple size must equal your given element count.
           []: list type, the same as tuple type: ().
           {key:value}: dictionary type.
           
        The format examples:
               iiS
               (i)
               (U)
               [i]
               {i:S}
               {i:(C<int>)}
               ([SC<int>NA(i)]{int:S}B
        """
        return self.__unpack(fmt)

    def unpackone(self, fmt):
        return self.__unpack(fmt)[0]

    def unpackcls(self, cls):
        return llbc.inl.PyStreamRead(self.__c_obj, cls)

    def unpacknone(self):
        return llbc.inl.PyStreamRead_None(self.__c_obj)

    def unpackbyte(self):
        return llbc.inl.PyStreamRead_Byte(self.__c_obj)

    def unpackbool(self):
        return llbc.inl.PyStreamRead_Bool(self.__c_obj)

    def unpackint16(self):
        return llbc.inl.PyStreamRead_Int16(self.__c_obj)

    def unpackint32(self):
        return llbc.inl.PyStreamRead_Int32(self.__c_obj)

    def unpackint64(self):
        return llbc.inl.PyStreamRead_Int64(self.__c_obj)

    def unpackfloat(self):
        return llbc.inl.PyStreamRead_Float(self.__c_obj)

    def unpackdouble(self):
        return llbc.inl.PyStreamRead_Double(self.__c_obj)

    def unpackstr(self):
        return llbc.inl.PyStreamRead_Str(self.__c_obj)

    def unpackstr2(self):
        return llbc.inl.PyStreamRead_Str2(self.__c_obj)

    def unpackunicode(self):
        return llbc.inl.PyStreamRead_Unicode(self.__c_obj)

    def unpackbytearray(self):
        return llbc.inl.PyStreamRead_ByteArray(self.__c_obj)

    def unpackbuffer(self):
        return llbc.inl.PyStreamRead_Buffer(self.__c_obj)

    def unpackstream(self, begin=0, end=-1):
        return llbc.inl.PyStreamRead_Stream(self.__c_obj, begin, end)

    def pack(self, fmt, *values):
        """
        Pack values according to the given format, the arguments must match the values required by the format exactly.
        format strings:
           c: char value(like b).
           b: byte value(like c).

           B: boolean value.
           s: short value.
           i: integer value.
           q: signed long long value.
           f: float value.
           d: double value(only support Fomat method).

           S: string value, auto append '\0' charcter.
           S#: string value, use another pack/unpack algorithm, 4 bytes length + string content(not include NULL character).
           U: unicode value.

           A: byte array value.
           F: buffer value.

           N: None value.

           C: class type, will automatic call class.encode() method to decode, must tell stream this class name, 
              use C<ClassName> semantic.
           (): tuple type, if only has one element,  it represent tuple all element type is the given type, otherwise
               the tuple size must equal your given element count.
           []: list type, the same as tuple type: ().
           {key:value}: dictionary type.
        """

        caller_env = None
        if fmt.find('C') >= 0 and not llbc.inl.PyStreamIsExprCompiled(fmt):
            caller_env = inspect.stack()[1][0].f_globals

        return llbc.inl.PyStreamFmtWrite(self.__c_obj, fmt, values, caller_env)

    def packobj(self, obj):
        return llbc.inl.PyStreamWrite(self.__c_obj, obj)

    def packnone(self):
        return llbc.inl.PyStreamWrite_None(self.__c_obj, None)

    def packbyte(self, obj):
        return llbc.inl.PyStreamWrite_Byte(self.__c_obj, obj)

    def packbool(self, obj):
        return llbc.inl.PyStreamWrite_Bool(self.__c_obj, obj)

    def packint16(self, obj):
        return llbc.inl.PyStreamWrite_Int16(self.__c_obj, obj)

    def packint32(self, obj):
        return llbc.inl.PyStreamWrite_Int32(self.__c_obj, obj)

    def packint64(self, obj):
        return llbc.inl.PyStreamWrite_Int64(self.__c_obj, obj)

    def packfloat(self, obj):
        return llbc.inl.PyStreamWrite_Float(self.__c_obj, obj)

    def packdouble(self, obj):
        return llbc.inl.PyStreamWrite_Double(self.__c_obj, obj)

    def packstr(self, obj):
        return llbc.inl.PyStreamWrite_Str(self.__c_obj, obj)

    def packstr2(self, obj):
        return llbc.inl.PyStreamWrite_Str2(self.__c_obj, obj)

    def packunicode(self, obj):
        return llbc.inl.PyStreamWrite_Unicode(self.__c_obj, obj)

    def packbytearray(self, obj):
        return llbc.inl.PyStreamWrite_ByteArray(self.__c_obj, obj)

    def packbuffer(self, obj):
        return llbc.inl.PyStreamWrite_Buffer(self.__c_obj, obj)

    def packtuple(self, obj):
        return llbc.inl.PyStreamWrite_Tuple(self.__c_obj, obj)

    def packlist(self, obj):
        return llbc.inl.PyStreamWrite_List(self.__c_obj, obj)

    def packsequence(self, obj):
        return llbc.inl.PyStreamWrite_Sequence(self.__c_obj, obj)

    def packdict(self, obj):
        return llbc.inl.PyStreamWrite_Dict(self.__c_obj, obj)

    def packstream(self, s, begin=0, to=-1):
        if not isinstance(s, pyllbcStream):
            raise TypeError('pack argument "s" must be stream type')
        return llbc.inl.PyStreamWrite_Stream(self.__c_obj, s.cobj, begin, to)

    def encode(self, s):
        if not isinstance(s, pyllbcStream):
            raise TypeError('encode argument not Stream type')

        return llbc.inl.PyStreamEncodeSelf(self.__c_obj, s.cobj)

    def __unpack(self, fmt, stack_idx=1):
        caller_env = None
        if fmt.find('C') >= 0 and not llbc.inl.PyStreamIsExprCompiled(fmt):
            caller_env = inspect.stack()[stack_idx + 1][0].f_globals

        return llbc.inl.PyStreamFmtRead(self.__c_obj, fmt, caller_env)

llbc.Stream = pyllbcStream

