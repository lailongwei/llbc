CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION
-------------------------------------------

Ask ``cmake_install.cmake`` script to error out as soon as a file with
absolute ``INSTALL DESTINATION`` is encountered.

The fatal error is emitted before the installation of the offending
file takes place.  This variable is used by CMake-generated
``cmake_install.cmake`` scripts.  If one sets this variable to ``ON`` while
running the script, it may get fatal error messages from the script.

.. versionadded:: 4.4

  The :diagnostic:`CMD_INSTALL_ABSOLUTE_DESTINATION` diagnostic can be used to
  generate errors for absolute install destinations at generate time.

See also :variable:`CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION`.
