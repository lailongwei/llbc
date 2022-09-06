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
using llbc;

using Console = llbc.SafeConsole;

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
class TimerTestComponent : IComponent
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

    private void _OnTimeout(Timer timer)
    {
        Console.WriteLine("Timeout handler called!");
    }

    private void _OnCancel(Timer timer)
    {
        Console.WriteLine("Timer cancel handler called!");
    }

    private Timer _timer;
}