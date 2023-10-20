# -*- coding: utf-8 -*-

import inspect

import llbc

__llbc_inl = llbc.inl

class pyllbcStream(object):
    """
    Stream class encapsulation, use to pack/unpack data sequence.
    """
    def __init__(self, cap=0, init_obj=None, endian=llbc.Endian.MachineEndian):
        self.__c_obj = __llbc_inl.Stream_New(self, cap, endian)
        self.packobj(init_obj)

    def __del__(self):
        __llbc_inl.Stream_Del(self.__c_obj)

    @property
    def endian(self):
        """
        Get stream endian setting(see llbc.Endian module).
        """
        return __llbc_inl.Stream_GetEndian(self.__c_obj)

    @endian.setter
    def endian(self, e):
        """
        Set stream endian(see llbc.Endian module).
        """
        __llbc_inl.Stream_SetEndian(self.__c_obj, e)

    @property
    def rpos(self):
        """
        Get stream read position.
        """
        return __llbc_inl.Stream_GetReadPos(self.__c_obj)

    @rpos.setter
    def rpos(self, p):
        """
        Set stream read position.
        """
        __llbc_inl.Stream_SetReadPos(self.__c_obj, p)

    @property
    def wpos(self):
        """
        Get stream write position.
        """
        return __llbc_inl.Stream_GetWritePos(self.__c_obj)

    @wpos.setter
    def wpos(self, p):
        """
        Set stream write position.
        """
        __llbc_inl.Stream_SetWritePos(self.__c_obj, p)

    @property
    def cap(self):
        """
        Get stream capacity.
        """
        return __llbc_inl.Stream_GetCap(self.__c_obj)

    @cap.setter
    def cap(self, s):
        """
        Set stream capacity.
        """
        __llbc_inl.Stream_SetCap(self.__c_obj, s)

    @property
    def raw(self):
        """
        Get stream memery view as buffer, buffer range[0, wpos).
        """
        return __llbc_inl.Stream_GetRaw(self.__c_obj)

    @raw.setter
    def raw(self, r):
        """
        Set stream raw memory from str/buffer/bytearray, wpos set to str/buffer/bytearray length.
        """
        __llbc_inl.Stream_SetRaw(self.__c_obj, r)

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
    def get_cached_compiled_expr_size():
        """
        Get cached compiled-expr size.
        """
        return __llbc_inl.Stream_GetCachedCompiledExprSize()

    @staticmethod
    def get_compiled_expr_cache_limit():
        """
        Get compiled-expr cache limit.
        """
        return __llbc_inl.Stream_GetCompiledExprCacheLimit()

    @staticmethod
    def set_compiled_expr_cache_limit(lmt):
        """
        Set compiled-expr cache limit.
        """
        __llbc_inl.Stream_SetCompiledExprCacheLimit(lmt)

    @staticmethod
    def is_expr_compiled(expr):
        """
        Check given expr compiled or not.
        """
        return __llbc_inl.Stream_IsExprCompiled(expr)

    @staticmethod
    def uncache_compiled_expr(expr):
        """
        Uncache compiled-expr.
        """
        __llbc_inl.Stream_UncacheCompiledExpr(expr)

    @staticmethod
    def uncache_all_compiled_exprs():
        """
        Uncache all compiled exprs.
        """
        __llbc_inl.Stream_UncacheAllCompiledExprs()

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
        caller_env = None
        if fmt.find('C') >= 0 and not __llbc_inl.Stream_IsExprCompiled(fmt):
            caller_env = inspect.stack()[1][0].f_globals

        return __llbc_inl.Stream_FmtRead(self.__c_obj, fmt, caller_env)


    def unpackcls(self, cls):
        return __llbc_inl.Stream_Read(self.__c_obj, cls)

    def unpacknone(self):
        return __llbc_inl.Stream_Read_None(self.__c_obj)

    def unpackbyte(self):
        return __llbc_inl.Stream_Read_Byte(self.__c_obj)

    def unpackbool(self):
        return __llbc_inl.Stream_Read_Bool(self.__c_obj)

    def unpackint16(self):
        return __llbc_inl.Stream_Read_Int16(self.__c_obj)

    def unpackint32(self):
        return __llbc_inl.Stream_Read_Int32(self.__c_obj)

    def unpackint64(self):
        return __llbc_inl.Stream_Read_Int64(self.__c_obj)

    def unpackfloat(self):
        return __llbc_inl.Stream_Read_Float(self.__c_obj)

    def unpackdouble(self):
        return __llbc_inl.Stream_Read_Double(self.__c_obj)

    def unpackstr(self):
        return __llbc_inl.Stream_Read_Str(self.__c_obj)

    def unpackstr2(self):
        return __llbc_inl.Stream_Read_Str2(self.__c_obj)

    def unpackunicode(self):
        return __llbc_inl.Stream_Read_Unicode(self.__c_obj)

    def unpackbytearray(self):
        return __llbc_inl.Stream_Read_ByteArray(self.__c_obj)

    def unpackbuffer(self):
        return __llbc_inl.Stream_Read_Buffer(self.__c_obj)

    def unpackstream(self, begin=0, end=-1):
        return __llbc_inl.Stream_Read_Stream(self.__c_obj, begin, end)

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
        if fmt.find('C') >= 0 and not __llbc_inl.Stream_IsExprCompiled(fmt):
            caller_env = inspect.stack()[1][0].f_globals

        return __llbc_inl.Stream_FmtWrite(self.__c_obj, fmt, values, caller_env)

    def packobj(self, obj):
        return __llbc_inl.Stream_Write(self.__c_obj, obj)

    def packnone(self):
        return __llbc_inl.Stream_Write_None(self.__c_obj, None)

    def packbyte(self, obj):
        return __llbc_inl.Stream_Write_Byte(self.__c_obj, obj)

    def packbool(self, obj):
        return __llbc_inl.Stream_Write_Bool(self.__c_obj, obj)

    def packint16(self, obj):
        return __llbc_inl.Stream_Write_Int16(self.__c_obj, obj)

    def packint32(self, obj):
        return __llbc_inl.Stream_Write_Int32(self.__c_obj, obj)

    def packint64(self, obj):
        return __llbc_inl.Stream_Write_Int64(self.__c_obj, obj)

    def packfloat(self, obj):
        return __llbc_inl.Stream_Write_Float(self.__c_obj, obj)

    def packdouble(self, obj):
        return __llbc_inl.Stream_Write_Double(self.__c_obj, obj)

    def packstr(self, obj):
        return __llbc_inl.Stream_Write_Str(self.__c_obj, obj)

    def packstr2(self, obj):
        return __llbc_inl.Stream_Write_Str2(self.__c_obj, obj)

    def packunicode(self, obj):
        return __llbc_inl.Stream_Write_Unicode(self.__c_obj, obj)

    def packbytearray(self, obj):
        return __llbc_inl.Stream_Write_ByteArray(self.__c_obj, obj)

    def packbuffer(self, obj):
        return __llbc_inl.Stream_Write_Buffer(self.__c_obj, obj)

    def packtuple(self, obj):
        return __llbc_inl.Stream_Write_Tuple(self.__c_obj, obj)

    def packlist(self, obj):
        return __llbc_inl.Stream_Write_List(self.__c_obj, obj)

    def packsequence(self, obj):
        return __llbc_inl.Stream_Write_Sequence(self.__c_obj, obj)

    def packdict(self, obj):
        return __llbc_inl.Stream_Write_Dict(self.__c_obj, obj)

    def packstream(self, s, begin=0, to=-1):
        if not isinstance(s, pyllbcStream):
            raise TypeError('pack argument "s" must be stream type')
        return __llbc_inl.Stream_Write_Stream(self.__c_obj, s.cobj, begin, to)

    def encode(self, s):
        if not isinstance(s, pyllbcStream):
            raise TypeError('encode argument not Stream type')

        return __llbc_inl.Stream_EncodeSelf(self.__c_obj, s.cobj)

llbc.Stream = pyllbcStream

