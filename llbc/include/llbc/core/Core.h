// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

// core/os
#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Atomic.h"
#include "llbc/core/os/OS_Library.h"
#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/os/OS_Process.h"
#include "llbc/core/os/OS_Console.h"
#include "llbc/core/os/OS_Socket.h"
#include "llbc/core/os/OS_Select.h"
#include "llbc/core/os/OS_Network.h"
#include "llbc/core/os/OS_Symbol.h"
#if LLBC_TARGET_PLATFORM_LINUX
#include "llbc/core/os/OS_Epoll.h"
#endif
#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/os/OS_Iocp.h"
#endif

// core/algo
#include "llbc/core/algo/RingBuffer.h"

// core/bundle
#include "llbc/core/bundle/Bundle.h"

// core/utils
#include "llbc/core/utils/Util_Math.h"
#include "llbc/core/utils/Util_Algorithm.h"
#include "llbc/core/utils/Util_Debug.h"
#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/utils/Util_Delegate.h"
#include "llbc/core/utils/Util_MD5.h"
#include "llbc/core/utils/Util_Base64.h"
#include "llbc/core/utils/Util_Misc.h"
#include "llbc/core/utils/Util_Network.h"
#include "llbc/core/utils/Util_Variant.h"

// core/helper
#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/helper/GUIDHelper.h"

// core/file
#include "llbc/core/file/File.h"
#include "llbc/core/file/Directory.h"

// core/variant
#include "llbc/core/variant/Variant.h"

// core/rapidjson
#include "llbc/core/rapidjson/json.h"

// core/tinyxml2
#include "llbc/core/tinyxml2/tinyxml2.h"

// core/config
#include "llbc/core/config/Ini.h"
#include "llbc/core/config/Property.h"

// core/time
#include "llbc/core/time/Time.h"

// core/event
#include "llbc/core/event/EventFirer.h"
#include "llbc/core/event/EventMgr.h"

// core/timer
#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerScheduler.h"

// core/thread
#include "llbc/core/thread/DummyLock.h"
#include "llbc/core/thread/SimpleLock.h"
#include "llbc/core/thread/RecursiveLock.h"
#include "llbc/core/thread/FastLock.h"
#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/thread/RWLock.h"
#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/ConditionVariable.h"
#include "llbc/core/thread/Semaphore.h"
#include "llbc/core/thread/Tls.h"
#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/thread/MessageBuffer.h"
#include "llbc/core/thread/MessageQueue.h"
#include "llbc/core/thread/ThreadMgr.h"
#include "llbc/core/thread/Task.h"

// core/singleton
#include "llbc/core/singleton/Singleton.h"

// core/log
#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerMgr.h"
#include "llbc/core/log/LogJsonMsg.h"

// core/random
#include "llbc/core/random/Random.h"

// core/entity
#include "llbc/core/entity/IHolder.h"
#include "llbc/core/entity/Entity.h"

// core/transcoder
#include "llbc/core/transcoder/Transcoder.h"

// core/library
#include "llbc/core/library/Library.h"

// core/objbase
#include "llbc/core/objbase/Object.h"
#include "llbc/core/objbase/ObjectMacro.h"
#include "llbc/core/objbase/ObjectFactory.h"
#include "llbc/core/objbase/Array.h"
#include "llbc/core/objbase/Dictionary.h"
#include "llbc/core/objbase/DictionaryElem.h"
#include "llbc/core/objbase/AutoReleasePool.h"
#include "llbc/core/objbase/AutoReleasePoolStack.h"

// core/objpool
#include "llbc/core/objectpool/ReferencablePoolObj.h"
#include "llbc/core/objectpool/ObjectPoolMgr.h"

__LLBC_NS_BEGIN

/**
 * LLBC library Core-Module startup function, before use any Core-Module's
 * functions or classes, must call this function to initialize the module.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_HIDDEN int __LLBC_CoreStartup();

/**
 * LLBC library Core-Module cleanup function.
 */
LLBC_HIDDEN void __LLBC_CoreCleanup();

__LLBC_NS_END
