SKIP_LINTING
------------

.. versionadded:: 4.4

This property allows you to exclude a source files of the specific file set
from the linting process. The linting process involves running
tools such as :prop_tgt:`<LANG>_CPPLINT`, :prop_tgt:`<LANG>_CLANG_TIDY`,
:prop_tgt:`<LANG>_CPPCHECK`, :prop_tgt:`<LANG>_ICSTAT`,
:prop_tgt:`<LANG>_PVS_STUDIO` and :prop_tgt:`<LANG>_INCLUDE_WHAT_YOU_USE` on
the source files, as well as compiling header files as part of
:prop_tgt:`VERIFY_INTERFACE_HEADER_SETS`. By setting ``SKIP_LINTING`` on a
file set, the mentioned linting tools will not be executed for the files of
that particular file set.  If the :prop_fs:`SKIP_LINTING` file set property
is set, it takes precedence over this source-file and target-wide properties.

This is a convenience alternative to setting the :prop_sf:`SKIP_LINTING`
source file property individually on each source.

Example
^^^^^^^

Consider a C++ project that includes multiple source files,
such as ``main.cpp``, ``things.cpp``, and ``generatedBindings.cpp``.
In this example, you want to exclude the ``things.cpp`` and
``generatedBindings.cpp`` files from the linting process. To achieve this, you
can use a file set and utilize the ``SKIP_LINTING`` property with the
:command:`set_property(FILE_SET) <set_property>` command as shown below:

.. code-block:: cmake

  add_executable(MyApp main.cpp)
  target_sources(MyApp PRIVATE FILE_SET skip_lint TYPE SOURCES
                               FILES things.cpp generatedBindings.cpp)

  set_property(FILE_SET skip_lint TARGET MyApp PROPERTY SKIP_LINTING ON)

In the provided code snippet, the ``SKIP_LINTING`` property is set to true
for the ``skip_lint`` file set. As a result, when the linting
tools specified by :prop_tgt:`<LANG>_CPPLINT`, :prop_tgt:`<LANG>_CLANG_TIDY`,
:prop_tgt:`<LANG>_CPPCHECK`, :prop_tgt:`<LANG>_ICSTAT` or
:prop_tgt:`<LANG>_INCLUDE_WHAT_YOU_USE` are executed, they will skip analyzing
the ``things.cpp`` and ``generatedBindings.cpp`` files.

By using the ``SKIP_LINTING`` file set property, you can selectively exclude
specific source files from the linting process. This allows you to focus the
linting tools on the relevant parts of your project, enhancing the efficiency
and effectiveness of the linting workflow.

See Also
^^^^^^^^

* :prop_sf:`SKIP_LINTING` source file property
* :prop_tgt:`SKIP_LINTING` target property
