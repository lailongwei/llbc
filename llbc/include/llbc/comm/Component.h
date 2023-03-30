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

#include "llbc/comm/ComponentEvents.h"

__LLBC_NS_BEGIN

/**
 * Previous declare Packet/Session/Service class.
 */
class LLBC_Packet;
class LLBC_Service;
class LLBC_SessionCloseInfo;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The session info class encapsulation.
 */
class LLBC_EXPORT LLBC_SessionInfo
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_SessionInfo();
    ~LLBC_SessionInfo();

public:
    /**
     * Confirm this create session is listen session.
     * @return bool - return true is this session is listen session, otherwise return false.
     */
    bool IsListenSession() const;
    /**
     * Set session listen flag.
     * @param[in] flag - the session listen flag.
     */
    void SetIsListenSession(bool flag);

    /**
     * Get the session Id.
     * @return int - the session Id.
     */
    int GetSessionId() const;
    /**
     * Set the session Id.
     * @param[in] sessionId - the session Id.
     */
    void SetSessionId(int sessionId);

    /**
     * Get accept session Id.
     * @return int - the accept session Id.
     */
    int GetAcceptSessionId() const;
    /**
     * Set accept sessionId.
     * @param[in] acceptSessionId - the accept session Id.
     */
    void SetAcceptSessionId(int acceptSessionId);

    /**
     * Get local address.
     * @return const LLBC_SockAddr_IN & - the local address.
     */
    const LLBC_SockAddr_IN &GetLocalAddr() const;
    /**
     * Set local address.
     * @param[in] addr - the local address.
     */
    void SetLocalAddr(const LLBC_SockAddr_IN &addr);

    /**
     * Get peer address.
     * @return const LLBC_SockAddr_IN & - the peer address.
     */
    const LLBC_SockAddr_IN &GetPeerAddr() const;
    /**
     * Set peer address.
     * @param[in] addr - the peer address.
     */
    void SetPeerAddr(const LLBC_SockAddr_IN &addr);

    /**
     * Get session socket handle.
     *   Note: Unsafe, if its not necessary, do not use it.
     * @return LLBC_SocketHandle - the socket handle.
     */
    LLBC_SocketHandle GetSocket() const;
    /**
     * Set session socket handle.
     * @param[in] handle - the socket handle.
     */
    void SetSocket(LLBC_SocketHandle handle);

public:
    /**
     * Get SessionInfo clas string representation.
     * @return LLBC_String - the session string representation.
     */
    LLBC_String ToString() const;

private:
    bool _isListen;
    int _sessionId;
    int _acceptSessionId;
    LLBC_SockAddr_IN _localAddr;
    LLBC_SockAddr_IN _peerAddr;
    LLBC_SocketHandle _sockHandle;
};

/**
 * \brief The session destroy information class encapsulation.
 */
class LLBC_EXPORT LLBC_SessionDestroyInfo
{
public:
    LLBC_SessionDestroyInfo(LLBC_SessionInfo *sessionInfo, 
                            LLBC_SessionCloseInfo *closeInfo);
    ~LLBC_SessionDestroyInfo();

public:
    /**
     * Get destroyed session information.
     */
    const LLBC_SessionInfo &GetSessionInfo() const;

    /**
     * Help methods about get session infos.
     */
    bool IsListenSession() const;
    int GetSessionId() const;
    int GetAcceptSessionId() const;
    LLBC_SocketHandle GetSocket() const;
    const LLBC_SockAddr_IN &GetLocalAddr() const;
    const LLBC_SockAddr_IN &GetPeerAddr() const;

public:
    /**
     * Is destroy from service layer flag getter.
     */
    bool IsDestroyedFromService() const;

    /**
    * Error No getter, available while IsDestroyedFromService() == false.
    */
    int GetErrno() const;

    /**
    * SubError No getter, available while IsDestroyedFromService() == false.
    */
    int GetSubErrno() const;

    /**
    * Reason getter.
    * Note: If is destroyed from service, it return Service destroy reason, otherwise return 
    *       network network layer error reason(Call LLBC_StrError() to get).
    */
    const LLBC_String &GetReason() const;

public:
    /**
     * Get the class object string representation.
     * @return LLBC_String - the string representation.
     */
    LLBC_String ToString() const;

private:
    LLBC_SessionInfo *_sessionInfo;
    LLBC_SessionCloseInfo *_closeInfo;
};

/**
 * \brief The asynchronous connect result info class encapsulation.
 */
class LLBC_EXPORT LLBC_AsyncConnResult
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_AsyncConnResult();
    ~LLBC_AsyncConnResult();

public:
    /**
     * Get connected flag.
     * @return bool - the connected flag.
     */
    bool IsConnected() const;
    /**
     * Set connected flag.
     * @param[in] connected - the connected flag.
     */
    void SetIsConnected(bool connected);

    /**
     * Get session Id.
     * @return int - the session Id.
     */
    int GetSessionId() const;
    /**
     * Set session Id.
     * @param[in] sessionId - the sessionId.
     */
    void SetSessionId(int sessionId);

    /**
     * Get last error.
     * @return const LLBC_String & - the reason.
     */
    const LLBC_String &GetReason() const;
    /**
     * Set reason.
     * @param[in] reason - the reason.
     */
    void SetReason(const LLBC_String &reason);

    /**
     * Get peer address.
     * @return const LLBC_SockAddr_IN & - the peer address.
     */
    const LLBC_SockAddr_IN &GetPeerAddr() const;
    /**
     * Set peer address.
     * @param[in] addr - the peer address.
     */
    void SetPeerAddr(const LLBC_SockAddr_IN &addr);

public:
    /**
     * Get this class object string representation.
     * @return LLBC_String - the string representation.
     */
    LLBC_String ToString() const;

private:
    int _sessionId;
    bool _connected;
    LLBC_String _reason;
    LLBC_SockAddr_IN _peerAddr;
};

/**
 * \brief The protocol stack report class encapsulation.
 */
class LLBC_EXPORT LLBC_ProtoReport
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_ProtoReport();
    ~LLBC_ProtoReport();

public:
    /**
     * Session getter & setter.
     */
    int GetSessionId() const;
    void SetSessionId(int sessionId);

    /**
     * Opcode getter & setter.
     */
    int GetOpcode() const;
    void SetOpcode(int opcode);

    /**
     * Layer getter & setter.
     */
    int GetLayer() const;
    void SetLayer(int layer);

    /**
     * Report level getter & setter.
     * To get more about level information, see ProtoReportLevel.h header file.
     */
    int GetLevel() const;
    void SetLevel(int level);

    /**
     * Report string getter & setter.
     */
    const LLBC_String &GetReport() const;
    void SetReport(const LLBC_String &report);

public:
    /**
     * Get this class string representation.
     * @return LLBC_String - the string representation.
     */
    LLBC_String ToString() const;

private:
    int _sessionId;
    int _opcode;

    int _layer;
    int _level;
    LLBC_String _report;
};

// Pre-declare LLBC_Component, use for define LLBC_ComponentMethod type.
class LLBC_Component;

/**
 * \brief The llibc library component method encapsulation.
 */
typedef LLBC_Delegate<int(const LLBC_Variant &, LLBC_Variant &)> LLBC_ComponentMethod;

/**
 * \brief The component methods encapsulation.
 */
class LLBC_ComponentMethods
{
public:
    typedef std::map<LLBC_CString, LLBC_ComponentMethod> Methods;
    typedef std::vector < std::pair<LLBC_CString, LLBC_ComponentMethod> > MethodList;

public:
    /**
     * Ctor & Dtor.
     */
    LLBC_ComponentMethods();
    ~LLBC_ComponentMethods();

public:
    /**
     * Get all component methods.
     * @return const Methods & - the methods dictionary.
     */
    const Methods &GetAllMethods() const;

    /**
     * Get component method.
     * @param[in] methName - the method name.
     * @return const LLBC_ComponentMethod & - the component method, if not found return nullptr.
     */
    const LLBC_ComponentMethod &GetMethod(const LLBC_CString &methName) const;

public:
    /**
     * Add component method.
     * @param[in] methName  - the method name.
     * @param[in] meth      - the component method.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddMethod(const LLBC_CString &methName, const LLBC_ComponentMethod &meth);

public:
    /**
     * Call component method.
     * @param[in] methName - the method name.
     * @param[in] arg      - the argument.
     * @param[in] ret      - the method execute result.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CallMethod(const LLBC_CString &methName, const LLBC_Variant &arg, LLBC_Variant &ret);

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_ComponentMethods);

private:
    Methods _meths;
    MethodList _methList;
};

/**
 * \brief The component interface class encapsulation.
 */
class LLBC_EXPORT LLBC_Component
{
public:
    explicit LLBC_Component(uint64 caredEvents = LLBC_ComponentEvents::DefaultEvents);
    virtual ~LLBC_Component();

public:
    /**
     * Check component inited or not.
     * @return bool - init flag.
     */
    bool IsInited() const;

    /**
     * Check component started or not.
     * @return bool - start flag.
     */
    bool IsStarted() const;

    /**
     * Get service.
     * @return LLBC_Service * - service object.
     */
    virtual LLBC_Service *GetService() const;

    /**
     * Get component.
     */
    template <typename Comp>
    typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value, Comp *>::type
    GetComponent();
    LLBC_Component *GetComponent(const LLBC_CString &compName);
    template <typename Comp>
    typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value, const Comp *>::type
    GetComponent() const;
    const LLBC_Component *GetComponent(const LLBC_CString &compName) const;

    /**
     * Get component list.
     */
    const std::vector<LLBC_Component *> &GetComponentList() const;

public:
    /**
     * Get cared events.
     * @return uint64 - the cared events.
     */
    uint64 GetCaredEvents() const;

    /**
     * Get cared specified components events or not.
     * @param[in] compEvs - the comp events(bit collection).
     * @return bool - return true it means cared specified events, otherwise return false.
     */
    bool IsCaredEvents(uint64 compEvs) const;

    /**
     * Get cared specified component events offset or not.
     * @param[in] compEvOffset - the comp event offset.
     * @return bool - return true it means cared specified event offset, otherwise return false.
     */
    bool IsCaredEventIndex(int compEvOffset) const;

public:
    /**
     * Get config type.
     * @return int - the config type, see LLBC_AppConfigType enum.
     */
    int GetConfigType() const;

    /**
     * Get non-property type config.
     * @return const LLBC_Variant & - the non-property application config.
     */
    const LLBC_Variant &GetConfig() const;

    /**
     * Set non-property type config(tempory support for t/master branch).
     * @param[in] compCfg - the component config.
     */
    void SetConfig(const LLBC_Variant &compCfg);

    /**
     * Get property type config.
     * @return const LLBC_Property & - the property config.
     */
    const LLBC_Property &GetPropertyConfig() const;

public:
    /**
     * Get all component methods.
     * @return const LLBC_ComponentMethods & - the component methods.
     */
    const LLBC_ComponentMethods &GetAllMethods() const;

    /**
     * Add component method(delegate method).
     * @param[in] methName - the method name.
     * @param[in] meth     - the method.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddMethod(const LLBC_CString &methName,
                  const LLBC_ComponentMethod &meth);

    /**
     * Add component method(component's member).
     * @param[in] methName - the method name.
     * @param[in] meth     - the method.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename Component>
    int AddMethod(const LLBC_CString &methName,
                  int (Component::*meth)(const LLBC_Variant &arg, LLBC_Variant &ret));

    /**
     * Call component method.
     * @param[in] methName - the method name.
     * @param[in] arg      - the argument.
     * @param[in] ret      - the method execute result.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int CallMethod(const LLBC_CString &methName,
                           const LLBC_Variant &arg, LLBC_Variant &ret);

public:
    /**
     * When service start and not not init component before, will call then event handler.
     */
    virtual bool OnInit(bool &finished);

    /**
     * When service destroy, will call this event handler.
     */
    virtual void OnDestroy(bool &finished);

public:
    /**
     * When service start, will call this event handler.
     */
    virtual bool OnStart(bool &finished);

    /**
     * When service all components started, will call this event handler.
     */
    virtual void OnLateStart(bool &finished);

    /**
     * When service all component will stop, will call this event handler.
     */
    virtual void OnEarlyStop(bool &finished);

    /**
     * When service stop, will call this event handler.
     */
    virtual void OnStop(bool &finished);

public:
    /**
     * Heartbeat function.
     */
    virtual void OnUpdate();

    /**
     * Idle event handler.
     * @param[in] idleTime - idle time. 
     */
    virtual void OnIdle(const LLBC_TimeSpan &idleTime);

public:
    /**
     * Application phase change event handler, when application start phase changed, will call these event handlers.
     */
    virtual void OnAppEarlyStart();
    virtual void OnAppStartFail();
    virtual void OnAppStartFinish();
    virtual void OnAppEarlyStop();

    /**
     * When application config reload, will call this event handler.
     */
    virtual void OnAppConfigReload();

public:
    /**
     * When new session create, will call this event handler.
     * @param[in] sessionInfo - the session info.
     */
    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo);

    /**
     * When session destroy, will call this event handler.
     * @param[in] destroyInfo - the session destroy info.
     */
    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo);

    /**
     * When asynchronous connect result, will call this event handler.
     * @param[in] result - asynchronous connect result data.
     */
    virtual void OnAsyncConnResult(const LLBC_AsyncConnResult &result);

public:
    /**
     * When protocol layer report something, will call this event handler.
     * @param[in] report - the report information.
     */
    virtual void OnProtoReport(const LLBC_ProtoReport &report);

    /**
     * When service receive a unhandled packet, will call this event handler.
     * @param[in] packet - the unhandled packet.
     */
    virtual void OnUnHandledPacket(const LLBC_Packet &packet);

private:
    /**
     * Friend class: LLBC_ServiceImpl.
     *  Access methods:
     *      void SetService();
     *      void UpdateComponentCfg();
     * Access data members:
     *      _inited;
     */
    friend class LLBC_ServiceImpl;

    /**
     * Set service.
     * @param[in] service - service object.
     */
    void SetService(LLBC_Service *service);

    /**
     * Update component config.
     */
    void UpdateComponentCfg();

private:
    bool _inited;
    bool _started;
    uint64 _caredEvents;

    LLBC_Service *_svc;
    LLBC_ComponentMethods *_meths;

    int _cfgType;
    LLBC_Property *_propCfg;
    LLBC_Variant *_nonPropCfg;
};

/**
 * \brief The component factory class encapsulation.
 */
class LLBC_EXPORT LLBC_ComponentFactory
{
public:
    virtual ~LLBC_ComponentFactory() {  }

public:
    virtual LLBC_Component *Create() const = 0;
};

__LLBC_NS_END

/**
 * Some stream output operator functions(in global ns).
 */
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_SessionInfo &si);
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_SessionDestroyInfo &destroy);
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_AsyncConnResult &result);
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_ProtoReport &report);

#include "llbc/comm/ComponentInl.h"


