/**
 * @file    TestCase_Comm_Service.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */

using System;
using llbc;

using Console = llbc.SafeConsole;

namespace TestCase_Comm_Timer
{
    public class TestCase_Comm_Timer : ITestCase
    {
        public void Run(string[] args)
        {
            Console.WriteLine("Timer test:");

            using (Service svc = new Service("TimerTestSvc"))
            {
                svc.Start();

                Console.WriteLine("Press any key to pause TimerTest...");
                Console.ReadKey();
            }
        }
    }

    [BindTo("TimerTestSvc")]
    class TimerTestFacade : IFacade
    {
        public override void OnStart()
        {
            // _timer = new Timer(_OnTimeout, _OnCancel);
            // _timer.Schedule(1.0, 0.5);

            _timer = Timer.Schedule(_OnTimeout, 1.0, 0, _OnCancel);
        }

        public override void OnStop()
        {
            _timer.Cancel();
        }

        private bool _OnTimeout(Timer timer)
        {
            Console.WriteLine("Timeout handler called!");
            return true;
        }

        private void _OnCancel(Timer timer)
        {
            Console.WriteLine("Timer cancel handler called!");
        }

        private Timer _timer;
    }
}