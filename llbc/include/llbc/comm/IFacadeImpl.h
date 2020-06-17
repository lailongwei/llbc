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

#ifdef __LLBC_COMM_IFACADE_H__

__LLBC_NS_BEGIN

inline LLBC_FacadeMethods::LLBC_FacadeMethods()
{
}

inline LLBC_FacadeMethods::~LLBC_FacadeMethods()
{
    LLBC_STLHelper::DeleteContainer(_meths);
}

inline const LLBC_FacadeMethods::Methods &LLBC_FacadeMethods::GetAllMethods() const
{
    return _meths;
}

inline LLBC_FacadeMethod *LLBC_FacadeMethods::GetMethod(const char *methName) const
{
    Methods::const_iterator it = _meths.find(methName);
    if (UNLIKELY(it == _meths.end()))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return it->second;
}

template <typename FacadeCls>
int LLBC_FacadeMethods::AddMethod(FacadeCls *facade, const char *methName, int ( FacadeCls::*meth)(const LLBC_Variant &arg, LLBC_Variant &ret))
{
    if (UNLIKELY(!methName || LLBC_StrLenA(methName) == 0 || !meth))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_FacadeMethod *methDeleg = new LLBC_Delegate2<
        int, FacadeCls, const LLBC_Variant &, LLBC_Variant &>(facade, meth);
    if (UNLIKELY(!_meths.insert(std::make_pair(methName, methDeleg)).second))
    {
        LLBC_Delete(methDeleg);
        LLBC_SetLastError(LLBC_ERROR_REPEAT);

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

inline int LLBC_FacadeMethods::CallMethod(const char *methName, const LLBC_Variant &arg, LLBC_Variant &ret)
{
    LLBC_FacadeMethod *meth = GetMethod(methName);
    if (UNLIKELY(!meth))
        return LLBC_FAILED;

    return meth->Invoke(arg, ret);
}

inline LLBC_IService *LLBC_IFacade::GetService() const
{
    return _svc;
}

inline uint64 LLBC_IFacade::GetCaredEvents() const
{
    return _caredEvents;
}

inline bool LLBC_IFacade::IsCaredEvents(uint64 facadeEvs) const
{
    return (_caredEvents & facadeEvs) == facadeEvs;
}

inline bool LLBC_IFacade::IsCaredEventOffset(int facadeEvOffset) const
{
 #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
    return IsCaredEvents(1Ui64 << facadeEvOffset);
#else
    return IsCaredEvents(1ULL << facadeEvOffset);
#endif
}

template <typename FacadeCls>
int LLBC_IFacade::AddMethod(const char *methName, int (FacadeCls::*meth)(const LLBC_Variant &arg, LLBC_Variant &ret))
{
    if (!_meths)
        _meths = LLBC_New(LLBC_FacadeMethods);

    return _meths->AddMethod<FacadeCls>(dynamic_cast<FacadeCls *>(this), methName, meth);
}

inline int LLBC_IFacade::CallMethod(const char *methName, const LLBC_Variant &arg, LLBC_Variant &ret)
{
    if (UNLIKELY(!_meths))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return _meths->CallMethod(methName, arg, ret);
}

inline void LLBC_IFacade::OnUpdate()
{
}

inline void LLBC_IFacade::OnIdle(int idleTime)
{
}

__LLBC_NS_END

#endif // __LLBC_COMM_IFACADE_H__
