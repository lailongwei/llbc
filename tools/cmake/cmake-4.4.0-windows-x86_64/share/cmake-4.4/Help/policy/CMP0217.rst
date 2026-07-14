CMP0217
-------

.. versionadded:: 4.4

The :prop_dir:`MACROS` directory property does not exist anymore.

In CMake 4.3 and below, the macros defined in a directory are registered in the
:prop_dir:`MACROS` directory property.  However, the property does not have the
same scope as macros, so it does not represent the actual list of macros.
In CMake 4.4 and above, this property is not longer filled with the defined
macros.  One may still use :command:`if(COMMAND myMacro) <if(COMMAND)>` to test
whether one exists.

The ``OLD`` behavior for this policy is to store macros definition in the
:prop_dir:`MACROS` directory property.
The ``NEW`` behavior for this policy is to no longer manage the
:prop_dir:`MACROS` directory property.

.. |INTRODUCED_IN_CMAKE_VERSION| replace:: 4.4
.. |WARNS_OR_DOES_NOT_WARN| replace:: does *not* warn
.. include:: include/STANDARD_ADVICE.rst

.. include:: include/DEPRECATED.rst
