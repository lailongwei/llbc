## 前置
感谢<a href="https://www.jetbrains.com"><img src="https://resources.jetbrains.com/storage/products/company/brand/logos/jb_beam.svg" alt="JetBrains Logo (Main) logo." style="width:5%;height:5%" /></a>为llbc提供的开发工具支持，推荐大家使用JetBrains开发工具进行开发。
llbc开发过程中，使用了以下JetBrains产品：
> <img src="https://resources.jetbrains.com/storage/products/company/brand/logos/ReSharper.png" alt="ReSharper logo." style="width:18%;height:18%" /><img src="https://resources.jetbrains.com/storage/products/company/brand/logos/ReSharperCPP.png" alt="ReSharper C++ logo." style="width:20%;height:20%" /> <img src="https://resources.jetbrains.com/storage/products/company/brand/logos/PyCharm.png" alt="PyCharm logo." style="width:15%;height:15%" /><img src="https://resources.jetbrains.com/storage/products/company/brand/logos/CLion.png" alt="CLion logo." style="width:15%;height:15%" /><img src="https://resources.jetbrains.com/storage/products/company/brand/logos/Space.png" alt="Space logo." style="width:15%;height:15%" />

> 如果你也是一名开源软件的开发者，可尝试申请JetBrains的[开源软件开发许可](https://www.jetbrains.com/shop/eform/opensource)。

> 欢迎加入Discord交流: Discord: https://discord.gg/YmtjKkNgaN

## 简述
一个简洁、高性能、跨平台、多语言支持的服务端开发框架，面向Service及Component，底层c++实现。
*Note：llbc框架不包含具体组件实现，所有较为通用的组件支持将由[llbc_comps](https://github.com/lailongwei/llbc_comps)仓库提供, llbc_comps仓库处于初建阶段，欢迎大家贡献组件。*

## 构建状态
[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
| 平台         | 编译器        | 类型                        | 状态                                                                                                                                                                     |
|-------------|---------------|-----------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Linux**   | g++, clang++  | Build, ~~Test~~, ~~Deploy~~ | [![Build Status](https://github.com/lailongwei/llbc/actions/workflows/linux-build.yml/badge.svg?branch=master)](https://github.com/lailongwei/llbc/actions/workflows/linux-build.yml) |
| **MacOS**   | g++, clang++  | Build, ~~Test~~, ~~Deploy~~ | [![Build Status](https://github.com/lailongwei/llbc/actions/workflows/macos-build.yml/badge.svg?branch=master)](https://github.com/lailongwei/llbc/actions/workflows/macos-build.yml) |
| **Windows** | Visual Studio | Build, ~~Test~~, ~~Deploy~~ | [![Build status](https://github.com/lailongwei/llbc/actions/workflows/windows-build.yml/badge.svg?branch=master)](https://github.com/lailongwei/llbc/actions/workflows/windows-build.yml) |  

## 文档
请点击 [llbc wiki](https://github.com/lailongwei/llbc/wiki)查阅，当前文档在完善中，也欢迎您贡献文档。

## 支持平台：
**Windows**: 提供稳定版本支持, 并保持所有特性处于最新状态, 可用于线上产品环境。  
**Linux**: 提供稳定版本支持, 并保持所有特性处于最新状态, 可用于线上产品环境。  
**Mac**: 提供稳定版本支持, 并保持所有特性处于最新状态, 但网络部分实现性能较低, 不建议用于线上产品环境。  
**iOS**: 提供稳定版本支持, 但特性无法保持最新, 可用于线上产品环境。  
**Android**: 理论上可编译并使用, 不提供稳定版本支持, 也无法保持最新。  

## 商业案例：
|[onemt--苏丹的复仇](http://www.ltjianhe.com/koh.html)|[onemt--苏丹传奇](https://play.google.com/store/apps/details?id=com.onemt.wolves.sos&hl=zh&gl=US)|[onemt--帝国时代](http://www.ltjianhe.com/boe.html)|[onemt--永恒之战](http://www.ltjianhe.com/woe.html)|[onemt--诸王黎明](http://www.ltjianhe.com/rok.html)|
|:--------:|:---------:|:-------:|:--------:|:--------:|
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__苏丹的复仇.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__苏丹传奇.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__帝国时代.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__永恒之战.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__诸王黎明.png)|
|[onemt--阿拉伯传奇](http://www.ltjianhe.com/dk.html)|[Seasun--天域幻想](https://www.925g.com/game01/20197.html)|[Seasun--剑仙奇侠传四](https://v.17173.com/zt/zjqbd/xj4/)|[游爱--剑仙传](https://baike.baidu.com/item/%E5%89%91%E4%BB%99%E4%BC%A0/3333767?fr=aladdin)|[游爱--掌上飞仙]()|
| ![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__阿拉伯传奇.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__seasun__天域幻想.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__seasun__仙剑4.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__游爱__剑仙传.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__游爱__掌上飞仙.png)|
|[37--天启之门](https://www.xiaogouh5.com/syrfrc/subuxyc.html)|||||
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase_37wan_天启之门.png)|||||

## 使用此框架的公司：
|[腾讯](https://www.tencent.com/zh-cn/)|[ONEMT](http://www.onemt.com/)|[西山居](https://www.xishanju.com/)|[祺曜互娱](http://www.7yao.top/)|[37wan](https://www.37.com/)|
|:--------------------------:|:----------------------------:|:--------------------------------:|:------------------------------:|:----------------------:|
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/tencent-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/onemt-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/seasun-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/qiyao-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/37-logo.jpg)|

## 支持：
llbc欢迎大家使用、提出问题及贡献代码。  
作者QQ：964855959  
作者Email：lailongwei@126.com, 964855959@qq.com  
QQ交流群：662590231  
手册：[llbc wiki](https://github.com/lailongwei/llbc/wiki)   


