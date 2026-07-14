Swift_PACKAGE_NAME
------------------

.. versionadded:: 4.4

Specify the Swift package name for a target.

This property sets the package name passed to the Swift compiler via the
``-package-name`` flag, which enables the package access control in Swift
code.

.. note::

   Package access control was added in Swift 5.8 and Xcode 15. This target
   property has no effect when using an earlier version of Swift or Xcode.

If the property is not set, the :variable:`PROJECT_NAME` variable is used
as the default package name.  See policy :policy:`CMP0216`.

Setting this property to the empty string will result in the target being
compiled without a package name.
