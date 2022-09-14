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

#ifdef __LLBC_APP_IAPPLICATION_H__

__LLBC_NS_BEGIN

inline LLBC_ApplicationEvent::LLBC_ApplicationEvent(int evType)
: evType(evType)
{
}

inline int LLBC_Application::OnWillStart(int argc, char *argv[])
{
    return LLBC_OK;
}

inline void LLBC_Application::OnStartFinish(int argc, char *argv[])
{
}

inline void LLBC_Application::OnWillStop()
{
}

inline void LLBC_Application::OnStopFinish()
{
}

inline void LLBC_Application::OnRun(bool &doNothing)
{
}

inline void LLBC_Application::OnConfigReload()
{
}

template <typename App>
inline App *LLBC_Application::ThisApp()
{
    return static_cast<App *>(_thisApp);
}

inline LLBC_Application *LLBC_Application::ThisApp()
{
    return _thisApp;
}

inline bool LLBC_Application::HasConfig() const
{
    return !_cfgPath.empty();
}

inline const LLBC_Property &LLBC_Application::GetPropertyConfig() const
{
    return _propCfg;
}

inline const LLBC_Variant &LLBC_Application::GetNonPropertyConfig() const
{
    return _nonPropCfg;
}

inline LLBC_ApplicationConfigType::ENUM LLBC_Application::GetConfigType() const
{
    return _cfgType;
}

inline const LLBC_String &LLBC_Application::GetConfigPath() const
{
    return _cfgPath;
}

inline bool LLBC_Application::IsStarted() const
{
    return _startThreadId != LLBC_INVALID_NATIVE_THREAD_ID;
}

inline int LLBC_Application::PushEvent(int evType)
{
    return PushEvent(new LLBC_ApplicationEvent(evType));
}

inline const LLBC_String &LLBC_Application::GetName() const
{
    return _name;
}

inline const LLBC_StartArgs &LLBC_Application::GetStartArgs() const
{
    return _startArgs;
}

inline LLBC_IService *LLBC_Application::GetService(int id) const
{
    return _services.GetService(id);
}

inline LLBC_IService *LLBC_Application::GetService(const LLBC_String &name) const
{
    return _services.GetService(name);
}

inline int LLBC_Application::StopService(int id, bool del)
{
    return _services.Stop(id, del);
}

inline int LLBC_Application::StopService(const LLBC_String &name, bool del)
{
    return _services.Stop(name, del);
}

__LLBC_NS_END

#endif // __LLBC_APP_IAPPLICATION_H__
