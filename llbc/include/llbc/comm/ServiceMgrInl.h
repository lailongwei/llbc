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

inline std::vector<int> LLBC_ServiceMgr::GetAllServiceIds() const
{
    std::vector<int> serviceIds;
    LLBC_LockGuard guard(_lock);
    LLBC_Foreach(_id2Services, serviceIds.push_back(item.first));

    return serviceIds;
}

inline LLBC_Strings LLBC_ServiceMgr::GetAllServiceNames() const
{
    LLBC_Strings serviceNames;
    LLBC_LockGuard guard(_lock);
    LLBC_Foreach(_name2Services, serviceNames.push_back(item.first));

    return serviceNames;
}

inline LLBC_Service *LLBC_ServiceMgr::GetServiceNonLock(int id)
{
    Id2Services::iterator it = _id2Services.find(id);
    return it != _id2Services.end() ? it->second : nullptr;
}

inline LLBC_Service *LLBC_ServiceMgr::GetServiceNonLock(const LLBC_CString &name)
{
    if (_serviceList.size() < 30)
    {
        std::vector<LLBC_Service *>::iterator it = std::find_if(
            _serviceList.begin(), _serviceList.end(), [name](LLBC_Service *const &svc)
        {
            return name == svc->GetName();
        });

        return it != _serviceList.end() ? *it : nullptr;
    }

    Name2Services::iterator it = _name2Services.find(name);
    return it != _name2Services.end() ? it->second : nullptr;
}

__LLBC_NS_END
