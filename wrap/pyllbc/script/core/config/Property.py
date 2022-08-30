# -*- coding: utf-8 -*-

import llbc

class pyllbcProperty(object):
    """
    pyllbc property class encapsulation.
    property can read & write .cfg format file.
    file format:
        path.to.name = value # The comments
        path.to.anotherName = \#\#\# anotherValue \#\#\#  # The comments too
    Property class can parse or serialize this format's file.
    """
    def __init__(self, prop_file=''):
        self._c_obj = llbc.inl.Property_New(prop_file)

    def __del__(self):
        llbc.inl.Property_Delete(self._c_obj)

    def __int__(self):
        """
        integer convert support.
        """
        return self.getvalue(as_type=int)

    def __float__(self):
        """
        float convert support.
        """
        return self.getvalue(as_type=float)

    def __str__(self):
        """
        str convert support.
        """
        return llbc.inl.Property_GetValue(self._c_obj, name) or ''

    def __unicode__(self):
        """
        unicode convert support.
        """
        return self.__str__().decode('utf8')

    def from_content(self, content):
        """
        Load properties from string content.
        :param content: the string content.
        """
        llbc.inl.Property_FromContent(self._c_obj, content)

    def from_file(self, prop_file):
        """
        Load properties from specifics property file.
        :param prop_file: the property file path.
        """
        llbc.inl.Property_FromFile(self._c_obj, prop_file)

    def to_content(self):
        """
        Serialize the properties as string.
        :return: the serialized properties' string representation.
        """
        return llbc.inl.Property_ToContent(self._c_obj)

    def to_file(self, prop_file):
        """
        Serialize the properties to file.
        :param prop_file: the property file path.
        """
        llbc.inl.Property_ToFile(self._c_obj, prop_file)

    def getvalue(self, name='', default=None, as_type=str):
        """
        Get property value by property name.
        examples:
            db_host = prop.getvalue("Server.DB.Host", "127.0.0.1")
            db_port = prop.getvalue("Server.DB.Port", default=3306, as_type=int)

            client_host = prop.get_property("Server.Client.Host").getvalue();
            client_port = prop.get_property("Server.Client.Port").getvalue(as_type=int);

            Note:
                If caller not pass the 'name' parameter or pass name parameter as '', will return
                self property's value, if this property is leaf-property will return real value,
                otherwise will return default value.
                If name format error, will raise llbc.error exception.
        :param name: the property, if pass a error format name, will raise llbc.error exception.
        :param default: default error when could not found specific name's property value, will
                        return this default. specially, if the default is None, the return value
                        is "as_type' is means return as_type().
        :param as_type: as type, Property class using str type to store all property values, if
                        not specific your type, will always return str type value.
        :return: the property value.
        """
        val = llbc.inl.Property_GetValue(self._c_obj, name)
        if val is None:
            return as_type() if default is None else default
        else:
            return as_type(val)
        
    def setvalue(self, name, value, comments=""):
        """
        Set property value.
        :param name: the property name.
        :param value: the property value, will convert str type to store.
        :param comments: the comments, default is empty.
        """
        llbc.inl.Property_SetValue(self._c_obj, name, value, comments)

    def get_comments(self, name=''):
        """
        Get specific property's comments, if name is '', return self property's comments.
        :param name: property name.
        :return: the property comments.
        """
        return llbc.inl.Property_GetComments(self._c_obj, name)

    def set_comments(self, name, comments):
        """
        Set specific property's comments, if name is '', return self property's comments.
        :param name: property name.
        :param comments: property comments.
        :return: None.
        """
        llbc.inl.Property_SetComments(self._c_obj, name, comments)

    def get_property(self, name):
        """
        Get the property.
        """
        return llbc.inl.Property_GetProperty(self._c_obj, name)

    def get_properties_count(self):
        """
        Get properties count.
        :return: the properties count.
        """
        return llbc.inl.Property_GetPropertiesCount(self._c_obj)

    def get_property_names(self, nest=False):
        """
        Get property names.
        :param nest: get all left-properties names or not, default is False.
        :return: the property names.
        """
        return llbc.inl.Property_GetPropertyNames(self._c_obj, nest)

    def has_property(self, name):
        """
        Check this property has specific name's property or not.
        :param name: the property name.
        :return: return True if has specific name's property, otherwise return False.
        """
        return llbc.inl.Property_HasProperty(self._c_obj, name)

    def remove_property(self, name, remove_all=True):
        """
        Remoev specific name's property.
        :param name: the property name.
        :param remove_all: remove all flag.
        """
        llbc.inl.Property_RemoveProperty(self._c_obj, name, remove_all)

    def remove_all_properties(self):
        """
        Remove all properties.
        """
        llbc.inl.Property_RemoveAllProperties(self._c_obj)

    def __getattr__(self, key):
        return llbc.inl.Property_GetProperty(self._c_obj, key)

    def __int__(self):
        return self.getvalue('', as_type=int)

    def __long__(self):
        return self.getvalue('', as_type=long)

    def __float__(self):
        return self.getvalue('', as_type=float)

    def __str__(self):
        return self.getvalue('', as_type=str)

    def __nonzero__(self):
        val = self.getvalue('', as_type=str)
        val_len = len(val)
        if val_len == 0:
            return False
        elif val_len == 4 and val.lower() == 'true':
            return True

        try:
            float_val = float(val)
        except Exception, e:
            return False
        else:
            return float_val != 0.0

    def __kvdict(self):
        return dict([(key, self.get_property(key)) for key in self.get_property_names(nest=True)])

    def iterkeys(self):
        return self.__kvdict().iterkeys()

    def itervalues(self):
        return self.__kvdict().itervalues()

    def iteritems(self):
        return self.__kvdict().iteritems()

    def __iter__(self):
        return self.__kvdict().__iter__()

llbc.Property = pyllbcProperty

