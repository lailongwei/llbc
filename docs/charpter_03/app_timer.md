# App及Timer - LLBC_App & LLBC_Timer
所有项目在写代码时，都会遇到第一个问题: **我要如何设计并定义我的`App`/`Server`对象，以整体管理服务器进程**, llbc提供了强大且简洁的App对象: `LLBC_App`， 开发者只需要继承它并实现`OnStart`、`OnStop`方法即可获得所有App管理相关的功能支持, 包括:
* 信号及信号处理, 如关服相关信号
* App配置管理及强大的Reload支持(直接完成App->Service->Compoenent)联动
* Crash捕获及处理, llbc App内置了强大的Crash捕获及处理
* 其它所有App相关的工作

我们所需要做的就只有：继承`LLBC_App`并实现相关事件接口即可。

以下代码展示了`LLBC_App`及`LLBC_Timer`功能：
```c++
#include <llbc.h>
using namespace llbc;

// 定义一个服务器对象, 继承LLBC_App即可, 它将托管你的主线程
// App对象将进行:
// - 信号处理工作: 如reload/stop信号处理
// - crash处理工作: crash捕获及处理, 包括框架及业务的安全退出支持
// - app级别配置读取工作: 框架强大的地方, 服务器/app 配置将被llbc框架加载并可直接使用
// - 其它所有app层相关的工作, 你甚至可以扩展它
class MyServer : public LLBC_App
{
public:
    int OnStart(int argc, char *argv[], bool &finished) override
    {
        // 定义timetout handler: 定时打印日志
        _timer.SetTimeoutHandler([&](LLBC_Timer *timer) {
            // 任何时候, 都可通过LLBC_App::ThisApp()获取到进程唯一的App对象
            LLBC_PrintLn("%s: Timer timeout, app ptr:%p",
                         LLBC_Time::Now().ToString().c_str(), LLBC_App::ThisApp<MyServer>());
        });

        // Schedule timer: 第一次1秒超时, 后面5秒超时(第二个参数不填写将使用第一个超时参数)
        _timer.Schedule(LLBC_TimeSpan::oneSec, LLBC_TimeSpan::oneSec * 5);

        // Log: app start
        LLBC_PrintLn("App start...");

        // 告诉app: 启动成功
        return LLBC_OK;
    }

    void OnStop(bool &finished) override
    {
        // Log: app stop
        LLBC_PrintLn("App stop...");

        // Cancel timer
        _timer.Cancel();
    }

private:
    // 定义一个timer
    LLBC_Timer _timer;
};

int main(int argc, char *argv[])
{
    // 定义MyServer实例并启动即可, LLBC_Startup/LLBC_Cleanup不需要再显式调用, LLBC_App将为你处理一切
    // 通过Ctrl+C可结束进程
    return MyServer().Start(argc, argv); 
}

// 程序输出:
// App start...
// 2024-11-27 21:45:15.987908: Timer timeout
// 2024-11-27 21:45:16.988402: Timer timeout
// 2024-11-27 21:45:21.990206: Timer timeout
// App stop...
```

代码解释:
* 通过定义`MyServer`类完成进程顶层类: App类 的定义，继承LLBC_App
* 通过重写`OnStart()`方法来实现服务器启动时要做的代码的编写，在示例代码中启动了一个5秒一次的timer
* 通过重写`OnStop()`方法来实现服务器关闭时要做的代码的编写
* 在`main()`函数中，不再需要`LLBC_Startup()/LLBC_Cleanup()`调用，简单定义一个`MyServer`对象并启动即可

代码完成的功能:
* 完成了App类对主线程的完整托管及Timer功能的实现

