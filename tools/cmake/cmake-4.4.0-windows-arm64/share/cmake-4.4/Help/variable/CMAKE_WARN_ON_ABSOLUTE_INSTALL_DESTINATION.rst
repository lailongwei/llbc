CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION
------------------------------------------

Ask ``cmake_install.cmake`` script to warn each time a file with absolute
``INSTALL DESTINATION`` is encountered.

This variable is used by CMake-generated ``cmake_install.cmake`` scripts.
If one sets this variable to ``ON`` while running the script, it may get
warning messages from the script.

.. versionadded:: 4.4

  The :diagnostic:`CMD_INSTALL_ABSOLUTE_DESTINATION` diagnostic can be used to
  generate warnings for absolute install destinations at generate time.

See also :variable:`CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION`.
