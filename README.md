## llbc
Low Level Base Compoment

llbc为一套轻量级的跨平台基础开发库,提供了跨平台开发中的所有基础支持(包括字符串处理,日志,线程,配置,Timer,ObjRef,网络及其它所有基础支持),在最顶层,llbc使用Service的概念将这些所有支持以compoments的方式集成,也就意味着,在开发你的应用的时候,只需要一个Service就拥有所有你需要的功能.
llbc是一个低调的库, 但~~~你开心就好.  

## 支持语言：
**c/c++**：完整支持, 并经过反复测试及线上使用
**python**: 完整支持, 并经过反复测试及线上使用, 在易用性上得益于python语法, 更容易学习及使用  
**csharp**: 基本支持, 并经过反复测试, 在易用性上同样得益于c#语法, 更容易学习及使用  
**lua**: 最小化支持, 包装库开发工作处于开始状态, 当前支持Log组件, 其它暂时未提供  

## 支持平台：
**Windows**: 提供稳定版本支持, 并保持所有特性处于最新状态, 可用于线上产品环境  
**Linux**: 提供稳定版本支持, 并保持所有特性处于最新状态, 可用于线上产品环境  
**Mac**: 提供稳定版本支持, 并保持所有特性处于最新状态, 但网络部分实现性能较低, 不建议用于线上产品环境  
**iOS**: 提供稳定版本支持, 但特性无法保持最新, 可用于线上产品环境  
**Android**: 理论上可编译并使用, 不提供稳定版本支持, 也无法保持最新

## 目录结构:  
> **llbc**: 核心库,使用c++语言编码  
> **testsuite**: 核心库测试套件  
> **wrap**: 其它语言封装库:  
>> **pyllbc**: 针对python语言的封装  
>> **csllbc**: 针对c#语言的封装  
>> **lullbc**: 针对lua语言的封装  
>
> **tools**: 工具目录, 为llbc库编译、测试、打包及其它需求提供支持, 开发者不需要理会:  
>> **premake**: llbc使用的编译系统及编译脚本, 编译系统使用premake  
>> **bulding_script**: llbc使用的编译脚本, 主要完成代码自动化构建及整合(包括native代码及各脚本代码)  
>> **old_compile_system**: llbc使用的旧式编译系统, 只作一个备份在此保存  

## 各项目说明:
llbc项目为一个整体性项目，里面按语言不同分成各个子项目，各个语言项目各自独立, 大家各取所需即可
> **c/c++**:
>> **llbc**: llbc核心库, 编译它可以在c++语言中直接使用
>> **testsuite**: llbc核心库测试套件, 可不编译, 如果需要测试及学习llbc, 建议编译
>
> **python**:
>> **pyllbc**: llbc python wrap库, 最终只生成一个llbc python库文件, 保证简洁, 如果需要将llbc用于您的python项目中, 请编译此项目
>
> **csharp**:
>> **csllbc_native**: llbc csharp wrap库实现中的native包装包, 用于包装符合c# PInvoke标准的接口实现
>> **csllbc**: llbc csharp wrap, 如果需要将llbc用于您的c#项目中, 请编译此项目(llbc_native项目会连同编译)
>> **csllbc_testsuite**: llbc csharp wrap库测试套件, 可不编译, testsuite项目一样, 如果需要测试及学习csllbc, 建议编译
> 
> **lua**:
>> **lullbc_lualib**: llbc lua wrap中的lua语言库项目, 最终生成指定版本的lualib
>> **lullbc_luaexec**: llbc lua wrap中的lua解释器, 最终生成lua.exe(windows)/lua(not windows)
>> **lullbc**: llbc lua wrap库, 最终只生成一个lullbc库及一个llbc.lua文件, 保证简洁, 如果需要将llbc用于您的lua项目中, 请编译此项目

## 编译:
得益于premake编译系统, 编译llbc库是一件非常简单的事情, 编译脚本会生成四个编译配置：
> debug32: 32位debug环境
> debug64: 64位debug环境
> release32: 32位release环境
> release64: 64位release环境
> 
请根据需求选择不同环境编译(windows请在项目属性中选择, linux及mac请在编译时使用config=xxxx来指定编译环境).
#### Windows:
运行WinPreBuild.bat, 生成指定vs版本sln文件, 打开并选择你要的语言项目编译.
当前支持的vs版本: **vs2005**, **vs2008**, **vs2010**, **vs2012**, **vs2013**, **vs2015**

#### Linux/Mac:
在linux环境下, 请确认是否已经安装libuuid(sudo yum install libuuid-devel)  
在Mac环境下, 请确认是否已经安装xCode command line tools  
> 编译llbc c++核心库: `make core_lib`
> 编译核心库测试套件: `make test`
> 编译pyllbc: `make py_wrap`
> 编译lullbc: `make lu_wrap`
> 编译csllbc: `make cs_wrap`
> 
ps: 直接`make`可以查看具体编译命令说明.

#### iOS:
使用xCode编译即可(untested)

#### Android:
未提供编译工具支持
  
## 安装:
### Windows:
所有目标文件在output/vsxxxx/<config>目录中, 在编译完成后, 在此目录提取即可.
    
### Linux/Mac
请根据你的需要执行安装指令:  
**llbc**: `make install_core_lib`  
**wrap/pyllbc**: `make install_py_wrap`  
**wrap/csllbc**: `make install_cs_wrap`, 为空实现, 如果需要使用csllbc, 请直接copy到您的目标目录即可   
**wrap/lullbc**: `make install_lu_wrap`, 为空实现, 如果需要使用lullbc, 请直接copy到你的目标目录即可
    
### iOS:
xCode编译出来dylib,直接在你的项目中引用即可
  
### Android:
没时间处理Android平台,暂时没有安装说明
