## 编译llbc
得益于[premake](https://github.com/premake/premake-core)工具，llbc的跨平台编译及同平台不同工具链版本的编译变得非常简单，在此强烈推荐使用[premake](https://github.com/premake/premake-core)工具去构建你的项目编译工具链，会比cmake之类的工具更简单，更易学习上手使用。

* **平台支持**：llbc当前主要支持的平台为`windows`，`linux`，`macosx`，另外两个手机平台`iOS`、`Android`理论上支持，但未测试（包括编译）
* **语言支持**：llbc框架核心库使用`c++`编写，故天然支持`c++`，`python`也完整支持，`c#`/`lua`受限于开发人员精力，未完整支持

### Step 1：初始化仓库
在编译前，先将llbc clone到本地，请在shell(非windows平台)中或者Git Bash(windows平台)中执行下面的bash代码：
```bash
# clone仓库到本地
git clone https://github.com/lailongwei/llbc.git
# 初始化submodules
cd llbc && git submodule update --init --recursive
```

### Step 2：大致了解各项目
* `llbc`： c++核心库库
* `tests/`：测试相关项目集
    * `testsuite`：核心测试项目
    * `quic_start`：quick start项目，用于快速入门
* `wrap/`：各语言包装库目录
    * `csllbc`：`c#`包装库
    * `lullbc`：`lua`包装库，如果需要修改lua版本，请更新submodule
    * `pyllbc`：`python`包装库（Note：受限于开发人员精力，当前python只支持 >= `python.2.7.18`的python2，python3不支持

Visual Studio项目视图：

<img width="511" height="270" alt="image" src="https://github.com/user-attachments/assets/cb5fd124-e1d8-4a10-afe9-04e620a3d790" />


### Step 3：编译
### 3.1 `Windows`平台编译
1. 于`llbc`项目根目录双击`WinPreBuild.bat`，选择你本机已安装的visual studio版本生成`sln`文件  
   <img width="445" height="182" alt="image" src="https://github.com/user-attachments/assets/7c7342ed-ab88-4dac-974e-24d5b49257cd" />
2. 生成后，`WinPreBuild.bat`脚本会自动打开`sln`文件生成目录，你只需要打开`llbc_vsxxxx.sln`文件即可
   <img width="853" height="382" alt="image" src="https://github.com/user-attachments/assets/83159be7-9f04-497b-a89a-0ab4822a4a5e" />
3. 选择正确的项目配置及架构，llbc在windows平台支持一个项目配置：
    * `debug32`：32位、debug版本编译配置，架构只能选择`x86`
    * `release32`：32位、release版本编译配置，架构只能选择`x86`
    * `debug64`：64位、debug版本编译配置，架构只能选择`x64`
    * `release64`：64位、release版本编译配置，架构只能选择`x64`
      以下是一个`release64 - x64`的选择示意图：
      <img width="657" height="295" alt="image" src="https://github.com/user-attachments/assets/240286f0-35b3-46e0-87ac-aa0422bf2fce" />
4. 选择你要的项目编译，或整个solution编译：
    * 如只想编译核心库：在`解决方案资源管理器`中，右击`llbc`项目，进行编译：

       <img width="480" height="413" alt="image" src="https://github.com/user-attachments/assets/42955456-e0e6-4067-bdd5-527aea82ace3" />
    * 如果想编译整个solution或其它project，右击solution或对应project项，并点击弹出菜单中的`生成`即可

编译Tips：
* 如果编译出现诡异问题, 请确认编译的**解决方案配置**跟**平台架构**是否配对, 以下配置将导致编译失败:
    * `release64` + `x86`
    * `debug64` + `x86`
    * `release32` + `x64`
    * `debug32` + `x64`

### 3.2 `Linux`/`MacOSX`平台编译
`Linux`/`MacOSX`平台编译较为简单，使用make工具即可快速编译，格式：
* `make <spec_target> [config=release64]`

编译Tips:
* 默认生成`release64`版本so/executable(s), 如果想编译`debug64`版本so/executable(s), 在运行make命令时, 增加`config=debug64`参数来生成

当前支持的target（可通过在`llbc/`项目目录支持`make`得到完成命令支持：
* `help`：显示帮助，默认执行此命令
* `all`：编译所有target，不建议，一般会报错，linux平台中的c#相关支持大部分机器不具备，建议编译特定目标即可
* `core_lib`：编译c++核心库
* `test`：编译`testsuite`
* `quick_start`：编译`quick_start`项目
* `wraps`：编译所有wraps，同样不建议
* `py_wrap`：编译pyllbc库(llbc python包装库)
* `lu_wrap`：编译lullbc库(llbc lua包装库)
* `cs_wrap`：编译csllbc库(llbc c#包装库)

### Step 4：提取`头文件`及`库文件`
llbc工程所有项目的编译结果文件输出到`output/xxxx/yyyy/`目录：
* `xxxx`：这个为当前你用的编译工具库名字，对于`Visual Studio`，将为`vsxxxx`，如: `vs2022`
* `yyyy`：为你选择的编译配置名，对于windows：`debug32`/`release32`/`debug64`/`release64`，对于非windows平台, 将只有`debug64`/`release64`平台

以下为windows平台的一个输出目录截图:

<img width="752" height="605" alt="image" src="https://github.com/user-attachments/assets/33add7a7-1f7d-4e1c-b87f-eda9f7a0edff" />

如果你使用c++核心库，头文件需要自行将`llbc/include`目录复制到你的目标项目目录，如你的目标项目是在`3rdparty/llbc/`中放llbc的核心库头文件, 那么最终目录结构如下:
* `3rdparty/llbc`
    * `include/`: llbc核心库include目录, 将此目录路径设置到你项目的预编译工具脚本中(如cmake`include_directories("3rdparty/llbc/include")`)
        * `llbc.h`: llbc统一对外头文件
        * `llbc/`
            * `common/`
            * `core/`
            * `...`
    * `lib/`
        * `release64/`: release64版本dll/so
        * `debug64/`: debug64版本dll/so
        * `.../`

## Hello World 示例

### 简述
以下示例展示最简单的llbc demo: `print Hello World`, 更多是让大家知道: 何初始化及清理llbc框架(llbc框架使用前要求初始化, 关闭前要求清理).

源码 - [quick_start - hello_world](https://github.com/lailongwei/llbc/tree/master/quick_start/hello_world)

### 代码及输出
```c++
// 包含llbc框架头文件, 并using
#include <llbc.h>
using namespace llbc;

int main(int argc, char *argv[])
{
    // 初始化llbc框架
    LLBC_Startup();
    // Defer清理llbc框架
    LLBC_Defer(LLBC_Cleanup());


    // Print hello world, 你也可以用std::cout/printf, 用LLBC_PrintLn省去你打\n
    LLBC_PrintLn("Hello World!");

    return 0; 
}
```

输出:

![01-hello_world-output](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/quick_start/01-hello_world-output.png)

* Print `Hello World!`

### 引入的知识点
* 如何在项目中包含llbc框架?
    * 简单`#include <llbc.h>`并`using namespace llbc`即可在项目中包含llbc框架.

* llbc框架是否需要初始化及清理? 如何初始化及清理?
    * llbc框架要求开发者显式初始化及清理;
    * 通过在`main()`函数第一行调用`LLBC_Startup()`、第二行进行`defer 调用LLBC_Cleanup()`即完成了llbc框架的初始化及清理工作.

* 如何输出`Hello World!`
    * 可通过`LLBC_PrintLn("Hello World!")`完成`Hello World!`的输出.

引入的函数简介:
* `LLBC_Startup()`: 完成框架初始化, 内部将会对框架内部所有模块、组件进行一次性初始化.
* `LLBC_Cleanup()`: 完成框架清理, 在进程退出前, 完成框架内部模块、组件的清理.
* `LLBC_PrintLn()`: 完成print line工作.