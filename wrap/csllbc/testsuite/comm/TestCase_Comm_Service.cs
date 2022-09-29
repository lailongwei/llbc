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
using System.Threading;
using System.Net;
using System.IO;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;

class TestCase_Comm_Service : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Comm/Service test:");

        // _BaseMethodsTest();
        _StartStopTest();
        // _ListenTest();
        // _ConnectTest();
        // _SendRecvTest();
        // _PreHandleTest();
        // _PacketExcTest();
        // _FrameExcTest();
        // _ExternalDriveTest();

        Console.WriteLine("Press any key to exit...");
        Console.ReadKey();
    }

    #region BaseMethods test
    private void _BaseMethodsTest()
    {
        Console.WriteLine("BaseMethods test:");

        using (Service svc = new Service("BaseMethodsTest"))
        {
            Console.WriteLine("DriveMode: {0}", svc.driveMode);
            Console.WriteLine("FPS: {0}", svc.fps);
            Console.WriteLine("frameInterval: {0}", svc.frameInterval);
            Console.WriteLine("isStarted: {0}", svc.isStarted);
            Console.WriteLine("svc type: {0}", svc.svcType);

            svc.fps = 1;
            Console.WriteLine("After set fps to 1, FPS: {0}, frameInterval: {1}", svc.fps, svc.frameInterval);

            try
            {
                svc.fps = LibConfig.commMaxServiceFPS + 10;
            }
            catch (Exception e)
            {
                Console.WriteLine("After set fps to {0}, exception occurred, right!", LibConfig.commMaxServiceFPS + 10);
                Console.WriteLine("exception:\n{0}", e);
            }
        }

        Console.WriteLine("Press any key to finish BaseMethods test...");
        Console.ReadKey();
    }
    #endregion

    #region StartStop test
    private void _StartStopTest()
    {
        Console.WriteLine("Start/Stop service test:");

        using (Service svc = new Service("StartStopTest"))
        {
            svc.Start();
            Console.WriteLine("Start service, wait 2 seconds...");
            Thread.Sleep(2000);

            Console.WriteLine("Stop service...");
            svc.Stop();

            Console.WriteLine("ReStart service...");
            svc.Start(2);
            Thread.Sleep(5000);

            Console.WriteLine("Stop service...");
            svc.Stop();
        }

        Console.WriteLine("Press any key to finish Start/Stop service test...");
        Console.ReadKey();
    }

    [BindTo("StartStopTest")]
    class TestComponentStartStop : IComponent
    {
        public override void OnInit()
        {
            _updateOutputTimes = 0;
            _idleOutputTimes = 0;
            Console.WriteLine("Service [{0}] init", svc.svcName);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service [{0}] destroy", svc.svcName);
        }

        public override void OnStart()
        {
            Console.WriteLine("Service [{0}] start", svc.svcName);
        }

        public override void OnStop()
        {
            Console.WriteLine("Service [{0}] stop", svc.svcName);
            _idleOutputTimes = 0;
            _updateOutputTimes = 0;
        }

        public override void OnUpdate()
        {
            if (_updateOutputTimes == 5)
                return;

            Console.WriteLine("Service [{0}] update", svc.svcName);
            _updateOutputTimes += 1;
        }

        public override void OnIdle(int idleTime)
        {
            if (_idleOutputTimes == 5)
                return;

            Console.WriteLine("Service [{0}] idle, idleTime(micro-seconds): {1}", svc.svcName, idleTime);
            _idleOutputTimes += 1;
        }

        private int _updateOutputTimes;
        private int _idleOutputTimes;
    }
    #endregion StartStop test

    # region Listen test
    private void _ListenTest()
    {
        Console.WriteLine("Listen test:");

        using (Service svc = new Service("ListenTest"))
        {
            svc.Start();

            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7788);

            int sessionId = svc.Listen(endPoint);
            Console.WriteLine("Listened on {0}, sessionId: {1}", endPoint, sessionId);

            Console.WriteLine("Remove listen session: {0}", sessionId);
            svc.RemoveSession(sessionId, "Listen test remove", true);

            Console.WriteLine("Press any key to exit listen test...");
            Console.ReadKey();
        }
    }

    [BindTo("ListenTest")]
    class TestComponentListenTest : IComponent
    {
        public override void OnInit()
        {
            Console.WriteLine("Service[{0}] init", svc.svcName);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service[{0}] destroy", svc.svcName);
        }

        public override void OnSessionCreate(SessionInfo sessionInfo)
        {
            Console.WriteLine("SessionCreate: {0}", sessionInfo);
        }

        public override void OnSessionDestroy(SessionDestroyInfo destroyInfo)
        {
            Console.WriteLine("SessionDestroy: {0}", destroyInfo);
        }
    }
    #endregion

    #region Connect test
    private void _ConnectTest()
    {
        Console.WriteLine("Connect test:");

        using (Service svc = new Service("ConnectTest"))
        {
            svc.Start();

            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7788);

            // Listen
            int listenSessionId = svc.Listen(endPoint);
            Console.WriteLine("Listened on {0}, sessionId: {1}", endPoint, listenSessionId);
            // Synchronous connect.
            int connSessionId = svc.Connect(endPoint);
            Console.WriteLine("Connected to {0}, sessionId: {1}", endPoint, connSessionId);
            // Asynchronous connect.
            svc.AsyncConn(endPoint);
            // Error endpoint's asynchronous connect.
            svc.AsyncConn(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7789));

            Console.WriteLine("Press any key to exit connect test...");
            Console.ReadKey();
        }
    }

    [BindTo("ConnectTest")]
    class TestComponentConnectTest : IComponent
    {
        public override void OnAsyncConnResult(AsyncConnResult asyncConnResult)
        {
            Console.WriteLine("Async-Connect result: {0}", asyncConnResult);
        }

        public override void OnSessionCreate(SessionInfo sessionInfo)
        {
            Console.WriteLine("New session create: {0}", sessionInfo);
        }

        public override void OnSessionDestroy(SessionDestroyInfo destroyInfo)
        {
            Console.WriteLine("Session destory: {0}", destroyInfo);
        }
    }
    #endregion

    #region SendRecv test
    private void _SendRecvTest()
    {
        Console.WriteLine("Send/Recv test:");
        using (Service svc = new Service("SendRecvTest"))
        {
            svc.Start();

            IPEndPoint ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7788);
            int listenSessionId = svc.Listen(ep);
            Console.WriteLine("Listened on [{0}], sessionId: {1}", ep, listenSessionId);

            int connSessionId = svc.Connect(ep);
            Console.WriteLine("Connected to {0}, sessionId: {1}", ep, connSessionId);

            // Test Send
            TestPacket data = new TestPacket();
            data.intVal = 10086;
            data.strVal = "Hello World";
            svc.Send(connSessionId, data);

            // Test Multicast
            TestMulticastPacket multicastData = new TestMulticastPacket();
            multicastData.intVal = 10010;
            multicastData.strVal = "Multicast data";
            List<int> multicastSessionIds = new List<int>();
            multicastSessionIds.Add(connSessionId);
            svc.Multicast(multicastSessionIds, multicastData);

            // Test Broadcast
            TestBroadcastPacket broadcastData = new TestBroadcastPacket();
            broadcastData.intVal = 8888;
            broadcastData.strVal = "Broadcast data";
            svc.Broadcast(broadcastData);

            // Test unhandled packet
            svc.Send(connSessionId, new UnHandledPacket());

            // Test coder not found
            MemoryStream unhandledStream = new MemoryStream();
            unhandledStream.Write(new byte[30], 0, 30);
            svc.Send(connSessionId, 10086, unhandledStream);

            Console.WriteLine("Press any key to exit Send/Recv test...");
            Console.ReadKey();
        }
    }

    [BindTo("SendRecvTest", "PreHandleTest", "PacketExcTest", "ExternalDriveTest")]
    class TestPacket : ICoder
    {
        public int intVal;
        public string strVal;

        public bool Encode(MemoryStream stream)
        {
            BinaryFormatter fmter = new BinaryFormatter();
            fmter.Serialize(stream, intVal);
            fmter.Serialize(stream, strVal);

            return true;
        }

        public bool Decode(MemoryStream stream)
        {
            BinaryFormatter fmter = new BinaryFormatter();
            intVal = (int)fmter.Deserialize(stream);
            strVal = fmter.Deserialize(stream) as string;

            return true;
        }
    }

    [BindTo("SendRecvTest")]
    class UnHandledPacket : ICoder
    {
        public bool Encode(MemoryStream stream)
        {
            return true;
        }

        public bool Decode(MemoryStream stream)
        {
            return true;
        }
    }

    [BindTo("SendRecvTest")]
    class TestMulticastPacket : TestPacket
    {
    }

    [BindTo("SendRecvTest")]
    class TestBroadcastPacket : TestPacket
    {
    }

    [BindTo("SendRecvTest")]
    class TestComponentSendRecvTest : IComponent
    {
        public override void OnInit()
        {
            Console.WriteLine("Service {0} init", svc.svcName);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service {0} destroy", svc.svcName);
        }

        public override void OnProtoReport(ProtoReport report)
        {
            Console.WriteLine("Proto report: {0}", report);
        }

        public override void OnUnHandledPacket(Packet packet)
        {
            Console.WriteLine("UnHandled packet: {0}", packet);
        }

        [Handler(typeof(TestPacket))]
        public void OnPacket(Packet packet)
        {
            Console.WriteLine("Recv packet: {0}", packet);
        }

        [Handler(typeof(TestMulticastPacket))]
        public void OnMulticastPacket(Packet packet)
        {
            Console.WriteLine("Recv multicast-packet: {0}", packet);
        }

        [Handler(typeof(TestBroadcastPacket))]
        public void OnBroadcastPacket(Packet packet)
        {
            Console.WriteLine("Recv broadcast packet: {0}", packet);
        }
    }
    #endregion

    #region PreHandle, UnifyPreHandle test
    private void _PreHandleTest()
    {
        Console.WriteLine("PreHandle test:");
        using (Service svc = new Service("PreHandleTest"))
        {
            svc.Start();

            IPEndPoint ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7788);
            Console.WriteLine("Listened on {0}", ep);

            int connSessionId = svc.Connect(ep);
            Console.WriteLine("Connected to {0}, sessionId: {1}", ep, connSessionId);

            TestPacket data = new TestPacket();
            data.intVal = 10086;
            data.strVal = "Hello World";
            svc.Send(connSessionId, data);

            Console.WriteLine("Press any key to exit PreHandle test...");
            Console.ReadKey();
        }
    }

    [BindTo("PreHandleTest")]
    class TestComponentPreHandleTest : IComponent
    {
        public override void OnInit()
        {
            Console.WriteLine("Service {0} init", svc.svcName);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service {0} destroy", svc.svcName);
        }

        [UnifyPreHandler]
        public bool OnUnifyPreHandle(Packet packet)
        {
            Console.WriteLine("OnUnifyPreHandle, packet: {0}", packet);
            return true;
        }

        [PreHandler(typeof(TestPacket))]
        public bool OnPreHandle(Packet packet)
        {
            Console.WriteLine("OnPreHandle, packet: {0}", packet);
            return true;
        }

        [Handler(typeof(TestPacket))]
        public void OnHandle(Packet packet)
        {
            Console.WriteLine("OnHandle, packet: {0}", packet);
        }
    }
    #endregion

    #region PacketException test
    private void _PacketExcTest()
    {
        Console.WriteLine("PacketExc test:");
        using (Service svc = new Service("PacketExcTest"))
        {
            svc.Start();

            IPEndPoint ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7788);
            Console.WriteLine("Listened on {0}", ep);

            int connSessionId = svc.Connect(ep);
            Console.WriteLine("Connected to {0}, sessionId: {1}", ep, connSessionId);

            TestPacket data = new TestPacket();
            data.intVal = 10086;
            data.strVal = "Hello World";
            svc.Send(connSessionId, data);

            Console.WriteLine("Press any key to exit PacketExc test...");
            Console.ReadKey();
        }
    }

    [BindTo("PacketExcTest")]
    class TestComponentPacketExcTest : IComponent
    {
        public override void OnInit()
        {
            Console.WriteLine("Service {0} init", svc.svcName);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service {0} destroy", svc.svcName);
        }

        public override void OnSessionDestroy(SessionDestroyInfo destroyInfo)
        {
            Console.WriteLine("Session destroy: {0}", destroyInfo);
        }

        [Handler(typeof(TestPacket))]
        public void OnPacket(Packet packet)
        {
            Console.WriteLine("OnPacket: {0}", packet);
            throw new Exception("Test exception, Yep!");
        }

        [PacketExcHandler(PacketHandlePhase.Handle)]
        public void OnPacketExcHandler(PacketExceptionInfo excInfo)
        {
            Console.WriteLine("Packet exception, phase: {0}, packet: {1}, e: {2}", excInfo.phase, excInfo.packet, excInfo.exception);
        }

        [FrameExcHandler]
        public void OnFrameException(Service svc, Exception e)
        {
            Console.WriteLine("Frame exception: {0}", e);
        }

        [PreHandler(typeof(TestPacket))]
        public bool OnPrePacket(Packet packet)
        {
            Console.WriteLine("OnPrePacket: {0}", packet);
            return true;
        }

        [UnifyPreHandler]
        public bool OnUnifyPrePacket(Packet packet)
        {
            Console.WriteLine("OnUnifyPrePacket: {0}", packet);
            return true;
        }
    }
    #endregion

    #region FrameExc test
    private void _FrameExcTest()
    {
        Console.WriteLine("FrameExc test:");
        using (Service svc = new Service("FrameExcTest"))
        {
            svc.Start();

            Console.WriteLine("Press any key to exit FrameExc test...");
            Console.ReadKey();
        }
    }

    [BindTo("FrameExcTest")]
    class TestComponentFrameExcTest : IComponent
    {
        public override void OnInit()
        {
            Console.WriteLine("Service {0} init(By Component)", svc.svcName);
            throw new Exception("Test Exception throw by OnInit!!!");
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service {0} destroy(By Component)", svc.svcName);
        }

        [FrameExcHandler]
        public void OnFrameExc(Service svc, Exception e)
        {
            Console.WriteErrorLine("Service {0} exception raised, exception: {1}", svc.svcName, e);
        }
    }

    [BindTo("FrameExcTest")]
    class TestComponentFrameExcTest2 : IComponent
    {
        public override void OnInit()
        {
            Console.WriteLine("Service {0} init(By Component2)", svc.svcName);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service {0} destroy(By Component2)", svc.svcName);
        }
    }
    #endregion

    #region External-Drive test
    private void _ExternalDriveTest()
    {
        Console.WriteLine("External-Drive test:");
        using (var svc = new Service("ExternalDriveTest"))
        {
            svc.driveMode = ServiceDriveMode.ExternalDrive;
            svc.Start();
            svc.fps = 10;

            Console.WriteLine("Service[{0}] startup, and set fps to: {1}", svc, svc.fps);

            // Self drive 30 seconds
            Console.WriteLine("Loop 300 times...");
            for (int i = 0; i < 300; ++i)
                svc.OnSvc(true);

            Console.WriteLine("Press any key to exit External-Drive test...");
            Console.ReadKey();
        }
    }

    [BindTo("ExternalDriveTest")]
    class SelfDriveTestComponent : IComponent
    {
        public override void OnInit()
        {
            Console.Write("Service[{0}] init", svc);
        }

        public override void OnDestroy()
        {
            Console.WriteLine("Service[{0}] destroy", svc);
        }

        public override void OnStart()
        {
            var ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7788);
            _connSessionId = svc.Connect(ep);
        }

        public override void OnUpdate()
        {
            var packet = new TestPacket();
            packet.intVal = 1;
            packet.strVal = "Hello, World";
            svc.Send(_connSessionId, packet);
        }

        [Handler(typeof(TestPacket))]
        private void OnPacket(Packet packet)
        {
            Console.WriteLine("Recv packet: {0}", packet);
        }

        private int _connSessionId;
    }
    #endregion
}
