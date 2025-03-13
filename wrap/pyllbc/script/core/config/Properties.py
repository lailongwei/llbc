# -*- coding: utf-8 -*-

import llbc

class pyllbcProperties(object):
    """
    pyllbc properties class encapsulation.
    property can load & save .cfg/.properties format file.
    file format:
        path.to.name = value # The comments
        path.to.anotherName = \#\#\# anotherValue \#\#\#  # The comments too
    """

    _load_from_string = llbc.inl.Properties_LoadFromString
    _load_from_file = llbc.inl.Properties_LoadFromFile
    _save_to_string = llbc.inl.Properties_SaveToString
    _save_to_file = llbc.inl.Properties_SaveToFile

    @classmethod
    def load_from_string(cls, properties_str):
        """
        Load properties from string.
        :param properties_str: the properties string.
        :return: the properties object.
        """
        return cls._load_from_string(properties_str)

    @classmethod
    def load_from_file(cls, properties_file_path):
        """
        Load properties from properties file.
        :param properties_file_path: the properties file path.
        :return: the properties object.
        """
        return cls._load_from_file(properties_file_path)

    @classmethod
    def save_to_string(cls, properties):
        """
        Save properties to string.
        :param properties: the properties object(must be a dict instance).
        :return: the properties string.
        """
        return cls._save_to_string(properties)

    @classmethod
    def save_to_file(cls, properties, properties_file_path):
        """
        Save properties to file.
        :param properties: the properties object(must be a dict instance).
        :param properties_file_path: the properties file path.
        :return: None
        """
        cls._save_to_file(properties, properties_file_path)


llbc.Properties = pyllbcProperties

