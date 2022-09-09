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


using System;
using System.Reflection;
using System.Collections.Generic;
using System.Threading;
using System.Text;

namespace llbc
{
    #region ServiceRegHolderBase
    /// <summary>
    /// All service register holder class base class encapsulation.
    /// </summary>
    internal class ServiceRegHolderBase
    {
    }
    #endregion

    #region ServiceRegHolderMethod
    /// <summary>
    /// The service register holder method encapsulation.
    /// <para>use for describe all service about register stubs.</para>
    /// </summary>
    internal class ServiceRegHolderMethod : ServiceRegHolderBase
    {
        public ServiceRegHolderMethod(MethodInfo mi)
        {
            methodInfo = mi;
        }

        /// <summary>
        /// Get already hold register informations validate or not.
        /// <para>If invalidate, will raise LLBCException</para>
        /// </summary>
        public void ValidateCheck()
        {
            if (isHandlerType)
            {
                int asHandlerCount = 0;
                if (asHandler)
                    asHandlerCount += 1;
                if (asPreHandler)
                    asHandlerCount += 1;
                if (asUnifyPreHandler)
                    asHandlerCount += 1;

                if (asHandlerCount > 1)
                    throw new LLBCException(
                        "Cannot be specified 'Handler/PreHandler/UnifyPreHandler' attributes at the same method '{0}'", methodInfo);

                if (isExcHandlerType || isDftExcHandlerType)
                    throw new LLBCException(
                        "Cannot be specified 'Handler/PreHandler/UnifyPreHandler/PacketExcHandler' attributes at the same method '{0}'", methodInfo);
            }
        }

        /// <summary>
        /// Set all already hold register informations to given service.
        /// </summary>
        /// <param name="svc">service object</param>
        /// <param name="obj">method's declare type object</param>
        public void RegisterToService(Service svc, ref object obj)
        {
            _RegisterHandler(svc, PacketHandlePhase.Handle, ref obj);
            _RegisterHandler(svc, PacketHandlePhase.PreHandle, ref obj);
            _RegisterHandler(svc, PacketHandlePhase.UnifyPreHandle, ref obj);

            _RegisterExcHandler(svc, PacketHandlePhase.Handle, false, ref obj);
            _RegisterExcHandler(svc, PacketHandlePhase.PreHandle, false, ref obj);
            _RegisterExcHandler(svc, PacketHandlePhase.UnifyPreHandle, false, ref obj);

            _RegisterExcHandler(svc, PacketHandlePhase.Handle, true, ref obj);
            _RegisterExcHandler(svc, PacketHandlePhase.PreHandle, true, ref obj);

            _RegisterFrameExcHandler(svc, ref obj);
        }

        #region Internal implementation
        private void _RegisterHandler(Service svc, PacketHandlePhase phase, ref object obj)
        {
            if (phase == PacketHandlePhase.Handle && asHandler)
            {
                _CreateObject(ref obj);
                svc.Subscribe(handlerOpcode, 
                    methodInfo.CreateDelegate(typeof(PacketHandler), obj) as PacketHandler);
            }
            else if (phase == PacketHandlePhase.PreHandle && asPreHandler)
            {
                _CreateObject(ref obj);
                svc.PreSubscribe(preHandlerOpcode,
                    methodInfo.CreateDelegate(typeof(PacketPreHandler), obj) as PacketPreHandler);
            }
            else if (phase == PacketHandlePhase.UnifyPreHandle && asUnifyPreHandler)
            {
                _CreateObject(ref obj);
                svc.UnifyPreSubscribe(
                    methodInfo.CreateDelegate(typeof(PacketUnifyPreHandler), obj) as PacketUnifyPreHandler);
            }
        }

        private void _RegisterExcHandler(Service svc, PacketHandlePhase phase, bool asDefault, ref object obj)
        {
            if (phase == PacketHandlePhase.Handle)
            {
                if (!asDefault && asExcHandler)
                {
                    _CreateObject(ref obj);
                    for (int i = 0; i < excHandlerOpcodes.Count; ++i)
                    {
                        svc.SetPacketExcHandler(PacketHandlePhase.Handle,
                            methodInfo.CreateDelegate(typeof(PacketExcHandler), obj) as PacketExcHandler, excHandlerOpcodes[i]);
                    }
                }
                else if (asDefault && asDftExcHandler)
                {
                    _CreateObject(ref obj);
                    svc.SetDftPacketExcHandler(PacketHandlePhase.Handle, 
                        methodInfo.CreateDelegate(typeof(PacketExcHandler), obj) as PacketExcHandler);
                }
            }
            else if (phase == PacketHandlePhase.PreHandle)
            {
                if (!asDefault && asExcPreHandler)
                {
                    _CreateObject(ref obj);
                    for (int i = 0; i < excPreHandlerOpcodes.Count; ++i)
                    {
                        svc.SetPacketExcHandler(PacketHandlePhase.PreHandle,
                            methodInfo.CreateDelegate(typeof(PacketExcHandler), obj) as PacketExcHandler, excPreHandlerOpcodes[i]);
                    }
                }
                else if (asDefault && asDftExcPreHandler)
                {
                    _CreateObject(ref obj);
                    svc.SetDftPacketExcHandler(PacketHandlePhase.PreHandle, 
                        methodInfo.CreateDelegate(typeof(PacketExcHandler), obj) as PacketExcHandler);
                }
            }
            else if (phase == PacketHandlePhase.UnifyPreHandle && asExcUnifyPreHandler)
            {
                _CreateObject(ref obj);
                svc.SetPacketExcHandler(PacketHandlePhase.UnifyPreHandle,
                    methodInfo.CreateDelegate(typeof(PacketExcHandler), obj) as PacketExcHandler, 0);
            }
        }

        private void _RegisterFrameExcHandler(Service svc, ref object obj)
        {
            if (asFrameExcHandler)
            {
                _CreateObject(ref obj);
                svc.SetFrameExceptionHandler(
                    methodInfo.CreateDelegate(typeof(FrameExceptionHandler), obj) as FrameExceptionHandler);
            }
        }

        private void _CreateObject(ref object obj)
        {
            if (obj == null)
                obj = Activator.CreateInstance(methodInfo.DeclaringType);
        }
        #endregion

        public MethodInfo methodInfo;

        public bool isHandlerType
        {
            get
            {
                return asHandler || asPreHandler || asUnifyPreHandler;
            }
        }

        public bool asHandler;
        public int handlerOpcode;
        public bool asPreHandler;
        public int preHandlerOpcode;
        public bool asUnifyPreHandler;

        public bool isExcHandlerType
        {
            get
            {
                return asExcHandler || asExcPreHandler || asExcUnifyPreHandler;
            }
        }

        public bool asExcHandler;
        public List<int> excHandlerOpcodes;
        public bool asExcPreHandler;
        public List<int> excPreHandlerOpcodes;
        public bool asExcUnifyPreHandler;

        public bool isDftExcHandlerType
        {
            get
            {
                return asDftExcHandler || asDftExcPreHandler;
            }
        }

        public bool asDftExcHandler;
        public bool asDftExcPreHandler;

        public bool asFrameExcHandler;
    }
    #endregion

    #region ServiceRegHolderClass
    /// <summary>
    /// Service register holder class encapsulation.
    /// </summary>
    internal class ServiceRegHolderClass : ServiceRegHolderBase
    {
        /// <summary>
        /// Construct a service register holder class information.
        /// </summary>
        /// <param name="cls"></param>
        public ServiceRegHolderClass(Type cls)
        {
            this.cls = cls;
        }

        /// <summary>
        /// Check this register holder class is registable to specific service or not.
        /// </summary>
        /// <param name="svc"></param>
        /// <returns></returns>
        public bool IsRegistableTo(Service svc)
        {
            return bindTos.Contains(svc.svcName);
        }

        /// <summary>
        /// Check all already hold register informations validate or not.
        /// <para>If not validate, will raise LLBCException</para>
        /// </summary>
        public void ValidateCheck()
        {
            foreach (var holderMethod in methods.Values)
                holderMethod.ValidateCheck();
        }

        /// <summary>
        /// Register all already hold register informations to given service.
        /// </summary>
        /// <param name="svc">service object</param>
        public void RegisterToService(Service svc)
        {
            object obj = null;
            if (!IsRegistableTo(svc))
                return;

            _AddComp(svc, ref obj);
            _AddCoder(svc, ref obj);
            _RegisterGlobalCoder(svc, ref obj);

            foreach (var holderMethod in methods.Values)
                holderMethod.RegisterToService(svc, ref obj);
        }

        #region Internal implementation
        private void _AddCoder(Service svc, ref object obj)
        {
            if (!asCoder)
                return;

            svc.AddCoder(coderOpcode, cls);
        }

        private void _AddComp(Service svc, ref object obj)
        {
            if (!asComp)
                return;

            _CreateObject(ref obj);
            svc.AddComponent(obj as IComponent);
        }

        private void _RegisterGlobalCoder(Service svc, ref object obj)
        {
            if (!asGlobalCoder)
                return;

            _CreateObject(ref obj);
            svc.RegisterGlobalCoder(obj as IGlobalCoder);
        }

        private void _CreateObject(ref object obj)
        {
            if (obj == null)
                obj = Activator.CreateInstance(cls);
        }
        #endregion

        public Type cls;

        public bool asComp;
        public bool asCoder;
        public int coderOpcode;
        public bool asGlobalCoder;
        public List<string> bindTos = new List<string>();

        public Dictionary<MethodInfo, ServiceRegHolderMethod> methods = new Dictionary<MethodInfo, ServiceRegHolderMethod>();
    }
    #endregion

    /// <summary>
    /// Register information holder collector class encapsulation.
    /// </summary>
    internal class RegHolderCollector
    {
        /// <summary>
        /// Collect specified assembly's llbc library register information.
        /// </summary>
        /// <param name="assembly">given assembly</param>
        /// <param name="async"></param>
        public static void Collect(Assembly assembly, bool async = true)
        {
            lock (_lock)
            {
                if (_collectedAssemblies.Contains(assembly.FullName) ||
                    _collectingAssemblies.Contains(assembly.FullName))
                    throw new LLBCException("Could not repeat collect assembly '{0}' information", assembly.FullName);

                _collectingAssemblies.Add(assembly.FullName);
                if (!async)
                {
                    _DoCollect(assembly);
                    return;
                }

                Thread thread = new Thread(
                    new ParameterizedThreadStart(_CollectThreadEntry));
                thread.Start(assembly);
            }
        }

        /// <summary>
        /// Check given assembly is not collect or in collecting/collected.
        /// </summary>
        /// <param name="assemblyName"></param>
        /// <returns></returns>
        public static bool IsNotCollect(string assemblyName)
        {
            lock (_lock)
            {
                return !_collectingAssemblies.Contains(assemblyName) && 
                    !_collectedAssemblies.Contains(assemblyName);
            }
        }

        /// <summary>
        /// Check given assembly is in collecting state or not.
        /// </summary>
        /// <param name="assemblyName"></param>
        /// <returns></returns>
        public static bool IsCollecting(string assemblyName)
        {
            lock (_lock)
            {
                return _collectingAssemblies.Contains(assemblyName);
            }
        }

        /// <summary>
        /// Check given assembly is collected state or not.
        /// </summary>
        /// <param name="assemblyName"></param>
        /// <returns></returns>
        public static bool IsCollected(string assemblyName)
        {
            lock (_lock)
            {
                return _collectedAssemblies.Contains(assemblyName);
            }
        }

        /// <summary>
        /// Register all already hold register informations to service.
        /// </summary>
        /// <param name="svc"></param>
        /// <param name="assembly"></param>
        public static void RegisterToService(Service svc, Assembly assembly = null)
        {
            if (assembly == null)
                assembly = LibIniter.loaderAssembly;

            string assemblyName = assembly.FullName;
            if (IsNotCollect(assemblyName))
            {
                Collect(assembly, false);
            }
            else
            {
                while (!IsCollected(assemblyName))
                    Thread.Sleep(5);
            }

            _RegisterToService(assemblyName, svc);
        }

        /// <summary>
        /// Get specific coder opcode.
        /// </summary>
        /// <param name="coder">coder</param>
        /// <returns>the coder opcode</returns>
        public static int GetCoderOpcode(Type coder)
        {
            CoderAttribute attr =
                coder.GetCustomAttribute(typeof(CoderAttribute), false) as CoderAttribute;
            if (attr == null || attr.autoGen)
            {
                if (attr == null && coder.GetInterface(typeof(ICoder).Name) == null)
                    throw new LLBCException("class '{0}' could not as a coder", coder);

                return StringUtil.UniqueId(coder.Name);
            }
            else
            {
                return attr.opcode;
            }
        }

        #region Internal implementation
        #region Collect about methods
        private static void _CollectThreadEntry(object _)
        {
            Assembly assembly = _ as Assembly;
            _DoCollect(assembly);
        }

        private static void _DoCollect(Assembly assembly)
        {
            ServiceRegHolderClass holderCls;

            Type[] types = assembly.GetTypes();
            for (int i = 0; i < types.Length; ++i)
            {
                Type ty = types[i];
                if (!ty.IsClass)
                    continue;
                else if ((holderCls = _CollectClass(ty)) == null)
                    continue;

                holderCls.ValidateCheck();
                lock (_lock)
                {
                    if (!_holders.ContainsKey(assembly.FullName))
                        _holders.Add(assembly.FullName, new List<ServiceRegHolderClass>());
                    _holders[assembly.FullName].Add(holderCls);
                }
            }

            lock (_lock)
            {
                _collectingAssemblies.Remove(assembly.FullName);
                _collectedAssemblies.Add(assembly.FullName);
            }
        }

        private static ServiceRegHolderClass _CollectClass(Type cls)
        {
            ServiceRegHolderClass holderCls = null;
            _DetectClass_Comp(cls, ref holderCls); // Component detect.
            _DetectClass_Coder(cls, ref holderCls); // ICoder detect.
            _DetectClass_GlobalCoder(cls, ref holderCls); // Global coder detect.
            
            // Collect all methods, only IComponent subclass can become service handlers.
            if (holderCls == null || !holderCls.asComp)
                return holderCls;

            MethodInfo[] clsMethods = null;
            _CollectMethods(cls, holderCls, ref clsMethods);

            return holderCls;
        }

        public static void _DetectClass_Comp(Type cls, ref ServiceRegHolderClass holderCls)
        {
            if (cls.BaseType != typeof(IComponent))
                return;

            _CreateHolderClass(cls, ref holderCls);
            holderCls.asComp = true;
        }

        private static void _DetectClass_Coder(Type cls, ref ServiceRegHolderClass holderCls)
        {
            if (cls.GetInterface(typeof(ICoder).Name) == null &&
                    cls.GetCustomAttribute(typeof(CoderAttribute), false)  == null)
                return;

            _CreateHolderClass(cls, ref holderCls);

            holderCls.asCoder = true;
            holderCls.coderOpcode = GetCoderOpcode(cls);
        }

        private static void _DetectClass_GlobalCoder(Type cls, ref ServiceRegHolderClass holderCls)
        {
            if (cls.GetInterface("IGlobalCoder") == null)
                return;

            _CreateHolderClass(cls, ref holderCls);
            holderCls.asGlobalCoder = true;
        }

        private static void _CreateHolderClass(Type cls, ref ServiceRegHolderClass holderCls)
        {
            if (holderCls != null)
                return;

            holderCls = new ServiceRegHolderClass(cls);
            holderCls.bindTos = _GetBindTos(cls);
        }

        private static void _GetClassMethods(Type cls, ref MethodInfo[] clsMethods)
        {
            if (clsMethods == null)
                clsMethods = cls.GetMethods(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.IgnoreCase);
        }

        private static void _CollectMethods(Type cls, ServiceRegHolderClass holderCls, ref MethodInfo[] clsMethods)
        {
            _GetClassMethods(cls, ref clsMethods);
            for (int i = 0; i < clsMethods.Length; ++i)
            {
                MethodInfo clsMethod = clsMethods[i];
                _DetectMethod_Handler(cls, clsMethod, PacketHandlePhase.Handle, holderCls);
                _DetectMethod_Handler(cls, clsMethod, PacketHandlePhase.PreHandle, holderCls);
                _DetectMethod_Handler(cls, clsMethod, PacketHandlePhase.UnifyPreHandle, holderCls);

                _DetectMethod_PacketExcHandler(cls, clsMethod, holderCls);

                _DetectMethod_FrameExcHandler(cls, clsMethod, holderCls);
            }
        }

        private static void _DetectMethod_Handler(Type cls, MethodInfo mi, PacketHandlePhase phase, ServiceRegHolderClass holderCls)
        {
            Type attrTy = (phase == PacketHandlePhase.Handle ? typeof(HandlerAttribute) : (
                phase == PacketHandlePhase.PreHandle ? typeof(PreHandlerAttribute) : typeof(UnifyPreHandlerAttribute)));
            Attribute attr = mi.GetCustomAttribute(attrTy, false);
            if (attr == null)
                return;

            if (phase == PacketHandlePhase.Handle)
            {
                ServiceRegHolderMethod holderMethod = _GetHolderMethod(holderCls, mi);
                holderMethod.asHandler = true;
                holderMethod.handlerOpcode = (attr as HandlerAttribute).opcode;
            }
            else if (phase == PacketHandlePhase.PreHandle)
            {
                ServiceRegHolderMethod holderMethod = _GetHolderMethod(holderCls, mi);
                holderMethod.asPreHandler = true;
                holderMethod.preHandlerOpcode = (attr as PreHandlerAttribute).opcode;
            }
            else
            {
                ServiceRegHolderMethod holderMethod = _GetHolderMethod(holderCls, mi);
                holderMethod.asUnifyPreHandler = true;
            }
        }

        private static void _DetectMethod_PacketExcHandler(Type cls, MethodInfo mi, ServiceRegHolderClass holderCls)
        {
            object[] attrs = mi.GetCustomAttributes(typeof(PacketExcHandlerAttribute), false);
            for (int i = 0; i < attrs.Length; ++i)
            {
                PacketExcHandlerAttribute attr = attrs[i] as PacketExcHandlerAttribute;

                ServiceRegHolderMethod holderMethod = _GetHolderMethod(holderCls, mi);
                if (attr.phase == PacketHandlePhase.Handle)
                {
                    if (attr.asDefault)
                    {
                        holderMethod.asDftExcHandler = true;
                    }
                    else
                    {
                        holderMethod.asExcHandler = true;
                        _MergeOpcodes(ref holderMethod.excHandlerOpcodes, attr.opcodes);
                    }
                }
                else if (attr.phase == PacketHandlePhase.PreHandle)
                {
                    if (attr.asDefault)
                    {
                        holderMethod.asDftExcPreHandler = true;
                    }
                    else
                    {
                        holderMethod.asExcPreHandler = true;
                        _MergeOpcodes(ref holderMethod.excPreHandlerOpcodes, attr.opcodes);
                    }
                }
                else
                {
                    holderMethod.asExcUnifyPreHandler = true;
                }
            }
        }

        private static void _DetectMethod_FrameExcHandler(Type cls, MethodInfo mi, ServiceRegHolderClass holderCls)
        {
            FrameExcHandlerAttribute attr = mi.GetCustomAttribute(
                typeof(FrameExcHandlerAttribute), false) as FrameExcHandlerAttribute;

            if (attr == null)
                return;

            ServiceRegHolderMethod holderMethod = _GetHolderMethod(holderCls, mi);
            holderMethod.asFrameExcHandler = true;
        }

        private static ServiceRegHolderMethod _GetHolderMethod(ServiceRegHolderClass holderCls, MethodInfo mi)
        {
            ServiceRegHolderMethod holderMethod;
            if (holderCls.methods.TryGetValue(mi, out holderMethod))
                return holderMethod;

            holderMethod = new ServiceRegHolderMethod(mi);
            holderCls.methods.Add(mi, holderMethod);

            return holderMethod;
        }

        private static List<string> _GetBindTos<T>(T ty) where T : ICustomAttributeProvider
        {
            object[] attrs = ty.GetCustomAttributes(typeof(BindToAttribute), false);
            List<string> bindTos = new List<string>(attrs.Length);
            for (int i = 0; i < attrs.Length; ++i)
            {
                List<string> oneBindTos = (attrs[i] as BindToAttribute).svcNames;
                if (oneBindTos == null)
                    continue;

                for (int j = 0; j < oneBindTos.Count; ++j)
                {
                    if (bindTos.Contains(oneBindTos[j]))
                        continue;

                    bindTos.Add(oneBindTos[j]);
                }
            }

            return bindTos;
        }

        private static void _MergeOpcodes(ref List<int> to, List<int> from)
        {
            if (to == null)
                to = new List<int>();

            for (int i = 0; i < from.Count; ++i)
            {
                if (to.Contains(from[i]))
                    continue;

                to.Add(from[i]);
            }
        }
        #endregion

        #region Register about methods
        private static void _RegisterToService(string assemblyName, Service svc)
        {
            lock (_lock)
            {
                List<ServiceRegHolderClass> holderClasses = null;
                if (!_holders.TryGetValue(assemblyName, out holderClasses))
                    return;

                for (int i = 0; i < holderClasses.Count; ++i)
                    holderClasses[i].RegisterToService(svc);
            }
        }
        #endregion
        #endregion

        private static object _lock = new object();

        private static HashSet<string> _collectedAssemblies = new HashSet<string>();
        private static HashSet<string> _collectingAssemblies = new HashSet<string>();

        private static Dictionary<string, List<ServiceRegHolderClass>> _holders = new Dictionary<string,List<ServiceRegHolderClass>>();
    }
}
