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

__LLBC_NS_BEGIN

inline LLBC_AppEvent::LLBC_AppEvent(int evType)
: evType(evType)
{
}


inline int LLBC_App::GetFPS() const
{
    return _fps;
}

inline bool LLBC_App::IsStopped() const
{
    return _startPhase == LLBC_AppStartPhase::Stopped;
}

inline bool LLBC_App::IsStarting() const
{
    return _startPhase == LLBC_AppStartPhase::Starting;
}

inline bool LLBC_App::IsStarted() const
{
    return _startPhase == LLBC_AppStartPhase::Started;
}

inline bool LLBC_App::IsStopping() const
{
    return _startPhase == LLBC_AppStartPhase::Stopping;
}

inline int LLBC_App::PushEvent(int evType)
{
    return PushEvent(new LLBC_AppEvent(evType));
}

inline LLBC_Service *LLBC_App::GetService(int id) const
{
    return _services.GetService(id);
}

inline LLBC_Service *LLBC_App::GetService(const LLBC_CString &name) const
{
    return _services.GetService(name);
}

inline int LLBC_App::StopService(int id, bool del, bool destroyComp)
{
    return _services.Stop(id, del, destroyComp);
}

inline int LLBC_App::StopService(const LLBC_CString &name, bool del, bool destroyComp)
{
    return _services.Stop(name, del, destroyComp);
}

__LLBC_NS_END
