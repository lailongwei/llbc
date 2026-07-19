CMAKE_Swift_SEPARATE_MODULE_EMISSION
-------------------------------------

.. versionadded:: 4.4

Specify whether Swift targets emit ``.swiftmodule`` from a dedicated build
edge.  This variable is used to initialize the
:prop_tgt:`Swift_SEPARATE_MODULE_EMISSION` property on targets as they are
created.

When set to ``ON``, importable Swift targets built by :ref:`Ninja Generators`
produce the ``.swiftmodule`` from a separate ``-emit-module`` build edge,
allowing downstream Swift targets to compile in parallel with upstream
object compilation.

If this variable is not set, separate module emission is enabled by default
in targets that do not set :prop_tgt:`Swift_SEPARATE_MODULE_EMISSION`.
See policy :policy:`CMP0215`.

.. note::

   Separate module emission takes effect only when policy :policy:`CMP0157`
   is set to ``NEW`` prior to the first :command:`project` or
   :command:`enable_language` command that enables the Swift language.
