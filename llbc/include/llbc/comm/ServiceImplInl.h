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

inline int LLBC_ServiceImpl::GetId() const
{
    return _id;
}

inline const LLBC_String &LLBC_ServiceImpl::GetName() const
{
    return _name;
}

inline int LLBC_ServiceImpl::GetConfigType() const
{
    return _cfgType;
}

inline const LLBC_Variant &LLBC_ServiceImpl::GetConfig() const
{
    return _cfg;
}

inline bool LLBC_ServiceImpl::IsFullStack() const
{
    return _fullStack;
}

inline LLBC_ServiceDriveMode::ENUM LLBC_ServiceImpl::GetDriveMode() const
{
    return _driveMode;
}

inline bool LLBC_ServiceImpl::IsStarted() const
{
    return _runningPhase == LLBC_ServiceRunningPhase::Started;
}

inline int LLBC_ServiceImpl::GetFPS() const
{
    return _fps;
}

inline int LLBC_ServiceImpl::GetFrameInterval() const
{

    return _fps != static_cast<int>(LLBC_INFINITE) ? 1000 / _fps : 0;
}

inline LLBC_EventMgr &LLBC_ServiceImpl::GetEventManager()
{
    return _evManager;
}

inline LLBC_ObjPool &LLBC_ServiceImpl::GetThreadSafeObjPool()
{
    return _threadSafeObjPool;
}

inline LLBC_ObjPool &LLBC_ServiceImpl::GetThreadUnsafeObjPool()
{
    return _threadUnsafeObjPool;
}

inline void LLBC_ServiceImpl::LockService()
{
    _lock.Lock();
}

inline void LLBC_ServiceImpl::UnlockService()
{
    _lock.Unlock();
}

__LLBC_NS_END
