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

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/BasicDataType.h"
#include "llbc/common/StringDataType.h"

__LLBC_NS_BEGIN
class LLBC_SockAddr_IN;
__LLBC_NS_END

/**
 * socket addr stream output operator previous declare.
 */
LLBC_EXTERN LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_SockAddr_IN &a);

__LLBC_NS_BEGIN

#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_LINUX
 typedef socklen_t LLBC_SocketLen;
 typedef int LLBC_SocketHandle;
 #define LLBC_INVALID_SOCKET_HANDLE (-1)
 typedef fd_set LLBC_FdSet;
#elif LLBC_TARGET_PLATFORM == LLBC_PLATFORM_WIN32
 typedef int LLBC_SocketLen;
 typedef SOCKET LLBC_SocketHandle;
 #define LLBC_INVALID_SOCKET_HANDLE INVALID_SOCKET
 typedef fd_set LLBC_FdSet;
#elif LLBC_TARGET_PLATFORM == LLBC_PLATFORM_IPHONE
 typedef socklen_t LLBC_SocketLen;
 typedef int LLBC_SocketHandle;
 #define LLBC_INVALID_SOCKET_HANDLE (-1)
 typedef fd_set LLBC_FdSet;
#elif LLBC_TARGET_PLATFORM == LLBC_PLATFORM_MAC
 typedef socklen_t LLBC_SocketLen;
 typedef int LLBC_SocketHandle;
 #define LLBC_INVALID_SOCKET_HANDLE (-1)
 typedef fd_set LLBC_FdSet;
#elif LLBC_TARGET_PLATFORM == LLBC_PLATFORM_ANDROID
 typedef socklen_t LLBC_SocketLen;
 typedef int LLBC_SocketHandle;
 #define LLBC_INVALID_SOCKET_HANDLE (-1)
 typedef fd_set LLBC_FdSet;
#endif

#if LLBC_TARGET_PLATFORM_NON_WIN32
 typedef int LLBC_IocpHandle;
 #define LLBC_INVALID_IOCP_HANDLE 0
 #define LLBC_IOCP_WAIT_INFINITE  0
#else // LLBC_TARGET_PLATFORM_NON_WIN32
 typedef HANDLE LLBC_IocpHandle;
 #define LLBC_INVALID_IOCP_HANDLE nullptr
 #define LLBC_IOCP_WAIT_INFINITE  WSA_INFINITE
#endif

#if LLBC_TARGET_PLATFORM_NON_WIN32
 struct LLBC_SockBuf
 {
     ulong len;
     char *buf;
 };
#else
 typedef WSABUF LLBC_SockBuf;
#endif

/**
 * \brief The internal socket address structure encapsulation.
 */
#pragma pack(push, 1)
class LLBC_EXPORT LLBC_SockAddr_IN
{
public:
    /**
     * Constructors.
     */
    LLBC_SockAddr_IN(uint16 addrFamily = AF_INET,
                     const char *ip = "127.0.0.1",
                     uint16 port = 0);
    LLBC_SockAddr_IN(const char *ip, uint16 port);

    /**
     * Copy constructor.
     */
    LLBC_SockAddr_IN(const LLBC_SockAddr_IN &right);

    /**
     * Destructor.
     */
    ~LLBC_SockAddr_IN();

public:
    /**
     * Convert to sockaddr_in data type.
     * @return struct sockaddr_in - OS specification socket address structure.
     */
    struct sockaddr_in ToOSDataType() const;

    /**
     * Convert from sockaddr_in data type.
     * @param[in] sockaddr - OS specification socket address structure.
     * @return int int - return 0 if success, otherwise return -1.
     */
    int FromOSDataType(const struct sockaddr_in *sockaddr);

    /**
     * Convert from sockaddr data type.
     * @param[in] sockaddr - OS specification socket address structure.
     * @param[in] len      - length.
     * @return int - return 0 if success, otherwise return -1.
     */
    int FromOSDataType(const struct sockaddr *sockaddr, LLBC_SocketLen len);

public:
    /**
     * Get address family.
     * @return uint16 - address family, like AF_INET.
     */
    uint16 GetAddressFamily() const;

    /**
     * Set address family.
     * @param[in] family - address family, must set to AF_INET.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetAddressFamily(uint16 family);

    /**
     * Get ip string.
     * @return LLBC_String - ip string.
     */
    LLBC_String GetIpAsString() const;

    /**
     * Get ip number, host byte order.
     * @return int - ip number, host byte order.
     */
    int GetIpAsNumber() const;

    /**
     * Get ip number, network byte order.
     * @return int - ip number, network byte order.
     */
    int GetIpAsNumberN() const;

    /**
     * Get IP address as in_addr.
     * @return struct in_addr - internet address.
     */
    struct in_addr GetIpAsINAddr() const;

    /**
     * Set ip from ip string.
     * @param[in] ip - ip string.
     */
    void SetIp(const LLBC_String &ip);

    /**
     * Set ip from ip number, host byte order.
     * @param[in] ip - ip number.
     */
    void SetIp(int ip);

    /**
     * Set ip from ip number, network byte order.
     * @param[in] ip - ip number.
     */
    void SetIpN(int ip);

    /**
     * Set ip from internet address.
     * @param[in] addr - internet address.
     */
    void SetIp(const struct in_addr &addr);

    /**
     * Get port number, host byte order.
     * @return uint16 - port number.
     */
    uint16 GetPort() const;

    /**
     * Get port number, network byte order.
     * @return uint16 - port number(network byte order).
     */
    uint16 GetPortN() const;

    /**
     * @Set port number, host byte order.
     * @param[in] port - port number.
     */
    void SetPort(uint16 port);

    /**
     * Set port number, network byte order.
     * @param[in] port - port number(network byte order).
     */
    void SetPortN(uint16 port);

    /**
     * Get padding buffer data.
     * @param[out]     buf - buffer pointer.
     * @param[in/out]  len - buffer length, must >= 8.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetPaddingBuf(void *buf, size_t &len);

    /**
     * Set padding buffer data.
     * @param[in] buf - buffer pointer.
     * @param[in] len - buffer length, must >= 8.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetPaddingBuf(const char *buf, size_t len);

    /**
     * Reset padding buffer.
     */
    void ZeroPaddingBuf();

public:
    /**
     * Returns a string representation of the object.
     * @return LLBC_String - a string representation of the object.
     */
    LLBC_String ToString() const;

public:
    /**
     * Operator = overlapped function.
     */
    LLBC_SockAddr_IN &operator =(const LLBC_SockAddr_IN &right);

    /**
     * Stream output operator.
     */
    friend std::ostream & ::operator <<(std::ostream &o, const LLBC_SockAddr_IN &a);

    /**
     * Operator == overlapped function.
     */
    bool operator ==(const LLBC_SockAddr_IN &right) const;

private:
    // Address family, like struct sockaddr_in.sim_family.
    uint16 _addrFamily;

    // Port number, network byte order.
    uint16 _port;
    // ip value, network byte order.
    int _ip;

    // Padding to make structure the same size as sockaddr.
    char _zero[8];
};
#pragma pack(pop)

/**
 * 
 * \brief Overlapped opcode encapsulation(WIN32 specific).
 */
class LLBC_OverlappedOpcode
{
public:
    /**
     * Opcode enumeration.
     */
    enum
    {
        Begin,

        Accept  = Begin,
        Connect,
        Receive,
        Send,

        End
    };
};

/**
 * \brief Overlapped structure encapsulation.
 */
struct LLBC_Overlapped
#if LLBC_TARGET_PLATFORM_WIN32
: public  OVERLAPPED
#endif // LLBC_TARGET_PLATFORM_WIN32 
{
    int opcode;
    LLBC_SocketHandle sock;
    LLBC_SocketHandle acceptSock;
    void *data;

    LLBC_Overlapped();
};
typedef struct LLBC_Overlapped * LLBC_POverlapped;

/**
 *\brief The overlapped group class encapsulation.
 */
class LLBC_OverlappedGroup
{
    typedef std::set<LLBC_POverlapped> _OLContainer;
    typedef _OLContainer::iterator _OLContainerIter;

public:
    typedef void(*DeleteDataProc)(void *data);

public:
    LLBC_OverlappedGroup();
    ~LLBC_OverlappedGroup();

public:
    /**
     * Get group socket handle.
     * @return LLBC_SocketHandle - socket handle.
     */
    LLBC_SocketHandle GetSocketHandle() const;

    /**
     * Set socket handle.
     * @param[in] handle - socket handle.
     */
    void SetSocketHandle(LLBC_SocketHandle handle);

    /**
     * Set delete data function.
     * @param[in] proc - function pointer.
     */
    void SetDeleteDataProc(DeleteDataProc proc);

    /**
     * Insert overlapped.
     * @param[in] ol - overlapped.
     */
    void InsertOverlapped(LLBC_POverlapped ol);

    /**
     * Delete overlapped.
     * @param[in] ol - overlapped.
     */
    void DeleteOverlapped(LLBC_POverlapped ol);

    /**
     * Delete all overlappeds.
     */
    void DeleteAllOverlappeds();

    /**
     * Remove overlapped.
     * @param[in] ol - overlapped.
     */
    void RemoveOverlapped(LLBC_POverlapped ol);

private:
    /**
     * Clear overlapped member datas.
     * @param[in] ol - overlapped.
     */
    void ClearOverlappedMembers(LLBC_POverlapped ol);

private:
    LLBC_SocketHandle _sock;
    _OLContainer _ols;

    DeleteDataProc _delDataProc;
};
typedef LLBC_OverlappedGroup * LLBC_POverlappedGroup;

__LLBC_NS_END
