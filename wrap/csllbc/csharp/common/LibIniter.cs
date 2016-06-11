/**
 * @file    LibIniter.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/26
 * @version 1.0
 *
 * @brief
 */

using System;
using System.Reflection;

namespace llbc
{
    /// <summary>
    /// llbc library initer, use for init/destroy llbc library.
    /// </summary>
    public static class LibIniter
    {
        /// <summary>
        /// Init llbc library.
        /// </summary>
        public static void Init(Assembly loaderAssembly)
        {
            if (loaderAssembly == null)
                throw new LLBCException("Loader assembly could not be null");
            else if (LLBCNative.csllbc_Startup() != 0)
                throw ExceptionUtil.CreateExceptionFromCoreLib();

            _loaderAssembly = loaderAssembly;
            RegHolderCollector.Collect(loaderAssembly, false);
        }

        /// <summary>
        /// Destroy llbc library.
        /// </summary>
        public static void Destroy()
        {
            if (LLBCNative.csllbc_Cleanup() != 0)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }

        /// <summary>
        /// Get llbc library loader assembly.
        /// </summary>
        public static Assembly loaderAssembly
        {
            get { return _loaderAssembly; }
        }

        private static Assembly _loaderAssembly;
    }
}