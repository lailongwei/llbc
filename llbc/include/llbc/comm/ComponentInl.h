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

inline LLBC_ComponentMethods::LLBC_ComponentMethods()
{
}

inline LLBC_ComponentMethods::~LLBC_ComponentMethods()
{
}

inline const LLBC_ComponentMethods::Methods &LLBC_ComponentMethods::GetAllMethods() const
{
    return _meths;
}

inline const LLBC_ComponentMethod &LLBC_ComponentMethods::GetMethod(const LLBC_CString &methName) const
{
    static const LLBC_ComponentMethod nullMeth;

    const size_t methSize = _methList.size();
    if (methSize <= 30)
    {
        for (size_t i = 0; i < methSize; ++i)
        {
            auto &methPair = _methList[i];
            if (methPair.first == methName)
                return methPair.second;
        }
    }
    else
    {
        Methods::const_iterator it = _meths.find(methName);
        if (LIKELY(it != _meths.end()))
            return it->second;
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return nullMeth;
}

inline int LLBC_ComponentMethods::AddMethod(const LLBC_CString &methName, const LLBC_ComponentMethod &meth)
{
    if (UNLIKELY(methName.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (UNLIKELY(!_meths.emplace(methName, meth).second))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _methList.emplace_back(methName, meth);

    return LLBC_OK;
}

inline int LLBC_ComponentMethods::CallMethod(const LLBC_CString &methName, const LLBC_Variant &arg, LLBC_Variant &ret)
{
    const LLBC_ComponentMethod &meth = GetMethod(methName);
    if (UNLIKELY(!meth))
        return LLBC_FAILED;

    return meth(arg, ret);
}

inline bool LLBC_Component::IsInited() const
{
    return _inited;
}

inline bool LLBC_Component::IsStarted() const
{
    return _started;
}

inline LLBC_Service *LLBC_Component::GetService() const
{
    return _svc;
}

template <typename Comp>
typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value, Comp *>::type
LLBC_Component::GetComponent()
{
    const auto &compList = GetComponentList();
    if (compList.size() <= 32)
    {
        Comp *castComp;
        for (auto &comp : compList)
        {
            if ((castComp = dynamic_cast<Comp *>(comp)) != nullptr)
                return castComp;
        }
    }

    #if LLBC_TARGET_PLATFORM_WIN32
    auto compName = typeid(Comp).name();
    #else
    auto compName = LLBC_GetTypeName(Comp);
    #endif
    const auto colonPos = strrchr(compName, ':');
    return static_cast<Comp *>(GetComponent(colonPos ? colonPos + 1 : compName));
}

template <typename Comp>
typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value, const Comp *>::type
LLBC_Component::GetComponent() const
{
    return const_cast<LLBC_Component *>(this)->GetComponent<Comp>();
}

inline const LLBC_Component *LLBC_Component::GetComponent(const LLBC_CString &compName) const
{
    return const_cast<LLBC_Component *>(this)->GetComponent(compName);
}

inline uint64 LLBC_Component::GetCaredEvents() const
{
    return _caredEvents;
}

inline bool LLBC_Component::IsCaredEvents(uint64 compEvs) const
{
    return (_caredEvents & compEvs) == compEvs;
}

inline bool LLBC_Component::IsCaredEventIndex(int compEvOffset) const
{
 #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
    return IsCaredEvents(1Ui64 << compEvOffset);
#else
    return IsCaredEvents(1ULL << compEvOffset);
#endif
}

inline int LLBC_Component::GetConfigType() const
{
    return _cfgType;
}

inline const LLBC_ComponentMethods &LLBC_Component::GetAllMethods() const
{
    static const LLBC_ComponentMethods emptyMethods;
    return LIKELY(_meths) ? *_meths : emptyMethods;
}

inline int LLBC_Component::AddMethod(const LLBC_CString &methName,
                                     const LLBC_ComponentMethod &meth)
{
    if (UNLIKELY(!_meths))
        _meths = new LLBC_ComponentMethods;

    return _meths->AddMethod(methName, meth);
}

template <typename Component>
int LLBC_Component::AddMethod(const LLBC_CString &methName,
                              int (Component::*meth)(const LLBC_Variant &arg, LLBC_Variant &ret))
{
    return AddMethod(methName, LLBC_ComponentMethod(dynamic_cast<Component *>(this), meth));
}

inline int LLBC_Component::CallMethod(const LLBC_CString &methName,
                                      const LLBC_Variant &arg, LLBC_Variant &ret)
{
    if (UNLIKELY(!_meths))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return _meths->CallMethod(methName, arg, ret);
}

inline bool LLBC_Component::OnInit(bool &finished)
{
    return true;
}

inline void LLBC_Component::OnDestroy(bool &finished)
{
}

inline bool LLBC_Component::OnStart(bool &finished)
{
    return true;
}

inline void LLBC_Component::OnLateStart(bool &finished)
{
}

inline void LLBC_Component::OnEarlyStop(bool &finished)
{
}

inline void LLBC_Component::OnStop(bool &finished)
{
}

inline void LLBC_Component::OnUpdate()
{
}

inline void LLBC_Component::OnIdle(const LLBC_TimeSpan &idleTime)
{
}

inline void LLBC_Component::OnAppEarlyStart()
{
}

inline void LLBC_Component::OnAppStartFail()
{
}

inline void LLBC_Component::OnAppStartFinish()
{
}

inline void LLBC_Component::OnAppEarlyStop()
{
}

inline void LLBC_Component::OnAppConfigReload()
{
}

inline void LLBC_Component::OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
{
}

inline void LLBC_Component::OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
{
}

inline void LLBC_Component::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
}

inline void LLBC_Component::OnProtoReport(const LLBC_ProtoReport &report)
{
}

inline void LLBC_Component::OnUnHandledPacket(const LLBC_Packet &packet)
{
}

inline void LLBC_Component::SetService(LLBC_Service *svc)
{
    _svc = svc;
}

__LLBC_NS_END
