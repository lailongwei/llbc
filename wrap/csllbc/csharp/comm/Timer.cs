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

namespace llbc
{
    #region Timer Timeout/Cancel handlers
    public delegate void TimeoutHandler(Timer timer);
    public delegate void TimerCancelHandler(Timer timer);
    #endregion

    /// <summary>
    /// Timer class.
    /// </summary>
    public class Timer
    {
        #region Ctor/Dtor
        /// <summary>
        /// Construct new timer.
        /// </summary>
        /// <param name="timeoutHandler">timeout handler</param>
        /// <param name="cancelHandler">timer cancel handler, optional</param>
        public Timer(TimeoutHandler timeoutHandler,
                     TimerCancelHandler cancelHandler = null)
        {
            _timeoutHandler = timeoutHandler;
            _cancelHandler = cancelHandler;

            _nativeTimeoutDeleg = _OnTimeout;
            _nativeCancelDeleg = _OnCancel;
            _nativeTimer = LLBCNative.csllbc_Timer_Create(_nativeTimeoutDeleg, _nativeCancelDeleg);
        }

        /// <summary>
        /// Destructor.
        /// </summary>
        ~Timer()
        {
            LLBCNative.csllbc_Timer_Destroy(_nativeTimer);
        }
        #endregion // Ctor/Dtor

        #region scheduling, dueTime/period
        /// <summary>
        /// Check this timer scheduling or not.
        /// </summary>
        public bool scheduling
        {
            get
            {
                return LLBCNative.csllbc_Timer_IsScheduling(_nativeTimer) != 0;
            }
        }

        /// <summary>
        /// Get timer dueTime(second).
        /// </summary>
        public double dueTime
        {
            get
            {
                return LLBCNative.csllbc_Timer_GetDueTime(_nativeTimer) / 1000.0;
            }
        }

        /// <summary>
        /// Get timer period(second).
        /// </summary>
        public double period
        {
            get
            {
                return LLBCNative.csllbc_Timer_GetPeriod(_nativeTimer) / 1000.0;
            }
        }
        #endregion // scheduling, dueTime/period

        #region timeoutHandler, cancelHandler
        /// <summary>
        /// Timer timeout handler.
        /// </summary>
        public TimeoutHandler timeoutHandler
        {
            get { return _timeoutHandler; }
        }

        /// <summary>
        /// Timer cancel handler.
        /// </summary>
        public TimerCancelHandler cancelHandler
        {
            get { return _cancelHandler; }
        }
        #endregion

        #region Schedule, Cancel
        /// <summary>
        /// Schedule timer.
        /// </summary>
        /// <param name="dueTime">dueTime, in seconds</param>
        /// <param name="period">period, in seconds, default is 0.0, means same with dueTime</param>
        public void Schedule(double dueTime, double period = 0.0)
        {
            int ret = LLBCNative.csllbc_Timer_Schedule(
                _nativeTimer, (long)(dueTime * 1000), (long)(period * 1000));
            if (ret != LLBCNative.LLBC_OK)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }

        /// <summary>
        /// Schedule timer.
        /// </summary>
        /// <param name="firstTimeoutTime">first timeout time, absolute time</param>
        /// <param name="period">period, in seconds, default is 0.0, means same with (firstTimeoutTime - DateTime.Now)</param>
        public void Schedule(DateTime firstTimeoutTime, double period = 0.0)
        {
            Schedule((firstTimeoutTime - DateTime.Now).TotalSeconds, period);
        }

        /// <summary>
        /// Schedule timer, same with Non-Static Schedule(double, double) method.
        /// </summary>
        /// <param name="timeoutHandler">Timeout handler</param>
        /// <param name="dueTime">dueTime, in seconds</param>
        /// <param name="period">period, in seconds</param>
        /// <param name="cancelHandler">timer cancel handler</param>
        /// <returns>new timer object</returns>
        public static Timer Schedule(TimeoutHandler timeoutHandler,
                                     double dueTime,
                                     double period = 0.0,
                                     TimerCancelHandler cancelHandler = null)
        {
            Timer timer = new Timer(timeoutHandler, cancelHandler);
            timer.Schedule(dueTime, period);

            return timer;
        }

        /// <summary>
        /// Schedule timer, same with Non-Static Schedule(DateTime, double) method.
        /// </summary>
        /// <param name="timeoutHandler">Timeout handler</param>
        /// <param name="firstTimeoutTime">first timeout time</param>
        /// <param name="period">period, in seconds</param>
        /// <param name="cancelHandler">timer cancel handler</param>
        /// <returns>new timer object</returns>
        public static Timer Schedule(TimeoutHandler timeoutHandler,
                                     DateTime firstTimeoutTime,
                                     double period = 0.0,
                                     TimerCancelHandler cancelHandler = null)
        {
            Timer timer = new Timer(timeoutHandler, cancelHandler);
            timer.Schedule(firstTimeoutTime, period);

            return timer;
        }

        /// <summary>
        /// Cancel timer.
        /// </summary>
        public void Cancel()
        {
            LLBCNative.csllbc_Timer_Cancel(_nativeTimer);
        }
        #endregion

        #region Internal implements
        private void _OnTimeout()
        {
            _timeoutHandler(this);
        }

        private void _OnCancel()
        {
            _cancelHandler(this);
        }
        #endregion // Internal implements

        TimeoutHandler _timeoutHandler;
        TimerCancelHandler _cancelHandler;

        IntPtr _nativeTimer;
        LLBCNative.Deleg_Timer_OnTimeout _nativeTimeoutDeleg;
        LLBCNative.Deleg_Timer_OnCancel _nativeCancelDeleg;
    }
}