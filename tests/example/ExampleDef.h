#pragma once

// Example registration list.
//
// IMPORTANT: the traits system derives each example id from
//   (__LINE__ - __EXAMPLE_ID_BEGIN), so every __DEFINE_EXAMPLE(...) MUST sit on
//   consecutive physical lines between __EXAMPLES_BEGIN() and __EXAMPLES_END()
//   -- do NOT insert blank or standalone-comment lines between them, otherwise
//   the menu indices become sparse. Grouping is expressed by ordering
//   (common -> core -> comm -> app, aligned with docs/_data/navigation.yml) and
//   by same-line trailing comments only.
//
// Register a new example with: __DEFINE_EXAMPLE("<menu name>", <class>)
__EXAMPLES_BEGIN()
__DEFINE_EXAMPLE("Hello World", Example_Common_HelloWorld)               // -- common --
__DEFINE_EXAMPLE("String", Example_Common_String)                        // common
__DEFINE_EXAMPLE("Error Handling", Example_Common_ErrorHandling)         // common
__DEFINE_EXAMPLE("Stream", Example_Core_Stream)                          // -- core --
__DEFINE_EXAMPLE("Variant", Example_Core_Variant)                        // core
__DEFINE_EXAMPLE("Delegate", Example_Core_Delegate)                      // core
__DEFINE_EXAMPLE("Object", Example_Core_Object)                          // core
__DEFINE_EXAMPLE("ObjPool", Example_Core_ObjPool)                        // core
__DEFINE_EXAMPLE("Objbase Collections", Example_Core_ObjbaseCollections) // core
__DEFINE_EXAMPLE("Logger", Example_Core_Logger)                          // core
__DEFINE_EXAMPLE("Timer", Example_Core_Timer)                            // core
__DEFINE_EXAMPLE("Time", Example_Core_Time)                              // core
__DEFINE_EXAMPLE("Thread", Example_Core_Thread)                          // core
__DEFINE_EXAMPLE("Event", Example_Core_Event)                            // core
__DEFINE_EXAMPLE("Config", Example_Core_Config)                          // core
__DEFINE_EXAMPLE("File & Directory", Example_Core_FileDirectory)         // core
__DEFINE_EXAMPLE("Random", Example_Core_Random)                          // core
__DEFINE_EXAMPLE("Library", Example_Core_Library)                        // core
__DEFINE_EXAMPLE("Transcoder", Example_Core_Transcoder)                  // core
__DEFINE_EXAMPLE("Utils", Example_Core_Utils)                            // core
__DEFINE_EXAMPLE("First Service", Example_Comm_FirstService)             // -- comm --
__DEFINE_EXAMPLE("Echo Service", Example_Comm_EchoService)               // comm
__DEFINE_EXAMPLE("Service Component", Example_Comm_ServiceComponent)     // comm
__DEFINE_EXAMPLE("Lifecycle & Event", Example_Comm_LifecycleEvent)       // comm
__DEFINE_EXAMPLE("Service Messaging", Example_Comm_ServiceMessaging)     // comm
__DEFINE_EXAMPLE("Packet & Coder", Example_Comm_PacketCoder)             // comm
__DEFINE_EXAMPLE("Session", Example_Comm_Session)                        // comm
__DEFINE_EXAMPLE("Protocol", Example_Comm_Protocol)                      // comm
__DEFINE_EXAMPLE("Poller", Example_Comm_Poller)                          // comm
__DEFINE_EXAMPLE("Multi-Service App", Example_App_MultiService)          // -- app --
__EXAMPLES_END()
