# llbc
Low Level Base Compoment

---

llbc为一套轻量级的跨平台基础开发库,提供了跨平台开发中的所有基础支持(包括字符串处理,日志,线程,配置,Timer,ObjRef,网络及其它所有基础支持),在最顶层,llbc使用Service的概念将这些所有支持以compoments的方式集成,也就意味着,在开发你的应用的时候,只需要一个Service就拥有所有你需要的功能.

**主要目录说明**:  
/llbc: 核心库,使用c++语言编码  
/testsuite: 核心库测试套件  
/wrap: 其它语言封装库  
/wrap/pyllbc: 针对python语言的封装  
/wrap/csllbc: 针对c#语言的封装  

*另*:  
  主要由于个人时间问题,文档基本没有,但testsuite中有足够的用例供大家参考,wrap/pyllbc/testsuite中也有足够的python用例可供参考.

##编译:
长期测试维护平台为win/linux平台,Mac,iOS,Android主要因为个人时间问题,难以长期维护(Android基本未编译测试,虽然代码中有此平台的兼容代码,Mac及iOS平台之前有过简单的编译测试)  
llbc库没有任何依赖可以直接安装,json代码文件使用jsoncpp,已经作调整,以支持64位int,并整合到库代码中.
###Windows:
运行WinPreBuild.bat,根据自己机器环境选择选择对应vs版本的sln文件及project文件,完成后进行打开sln文件进行编译即可.  
sln文件目标目录: build/vsxxxx, PreBuild脚本在生成后,会自动帮你打开此目录,只需要双击llbc_vsxxxx.sln编译即可.  
当前支持的vs版本: **vs2005**, **vs2008**, **vs2010**, **vs2012**, **vs2013**, **vs2015**
###Linux:
请确认是否已经安装libuuid(sudo yum install libuuid-devel)  
make all & make install
###Mac:
请确认是否已经安装xCode command line tools  
make all & make install(untested)
###iOS:
使用xCode编译即可(untested)
  
##安装:
###Windows:
llbc: 编译出的dll所在目录output/vsxxxx中,copy使用即可.  
testsuite: 编译出的测试套件在output/vsxxxx中,直接使用即可.  
wrap/pyllbc: python封装使用了自动化脚本,编译出来的python库将只有一个.pyd,编译输出目录为output/vsxxxx/wrap/python.  
*注*:  
> 如果你需要编译64位版本的llbc或者pyllbc,请在项目中调整你的编译配置即可,对于64位版本的pyllbc,请确认你机器使用的python为64位版本的python.  
> 如果你需要编译debug版本的llbc或者pyllbc,同样也在编译配置中调整即可.
    
###Linux & mac:
llbc: 编译出的so所在目录: llbc/lib,可以直接copy使用  
testsuite 编译出的可执行程序目录: testsuite/bin,直接使用  
pyllbc: 编译出的so所在目录为: wrap/pyllbc/lib,可以直接copy到你的python版本的site-package目录使用  
*注*:
> linux版本的makefile集成了install指令,建议使用make install安装
    
###iOS:
xCode编译出来dylib,直接在你的项目中引用即可
  
###Android:
没时间处理Android平台,暂时没有安装说明
