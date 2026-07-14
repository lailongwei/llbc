Swift_SEPARATE_MODULE_EMISSION
------------------------------

.. versionadded:: 4.4

Emit Swift modules separately from compilation in the :ref:`Ninja Generators`.

When this property is set to ``ON`` on an importable Swift target built by
:ref:`Ninja Generators`, CMake emits a separate ``-emit-module`` build edge
that produces the ``.swiftmodule`` independently of object compilation.
This allows downstream Swift targets to begin compiling as soon as the
module interface is available, rather than waiting for all upstream
compilation outputs.

The property is initialized from the value of the
:variable:`CMAKE_Swift_SEPARATE_MODULE_EMISSION` variable, if it is set.
If this property is not set, separate module emission is enabled by default.
See policy :policy:`CMP0215`.

.. note::

   Separate module emission takes effect only when policy :policy:`CMP0157`
   is set to ``NEW`` prior to the first :command:`project` or
   :command:`enable_language` command that enables the Swift language.
