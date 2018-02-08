/**
 * @file    ServiceMgrImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/07
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_SERVICE_MGR_H__

__LLBC_NS_BEGIN

inline LLBC_IService *LLBC_ServiceMgr::GetServiceNonLock(int id)
{
    _Services::iterator it = _id2Services.find(id);
    return it != _id2Services.end() ? it->second : NULL;
}

inline LLBC_IService *LLBC_ServiceMgr::GetServiceNonLock(const LLBC_String &name)
{
    _Services2::iterator it = _name2Services.find(name);
    return it != _name2Services.end() ? it->second : NULL;
}

__LLBC_NS_END

#endif // __LLBC_COMM_SERVICE_MGR_H__
