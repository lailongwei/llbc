---
layout: default
title: 安装与构建
---

# 安装与构建

llbc 提供三条构建路径，按你的目标平台选择即可。本页先教你把**核心库**编译出来，
后续的 [Hello World](hello-world.md) 会直接链接它。

- **premake → make**：Linux / macOS 首选。
- **CMake**：Linux / macOS 次选（仅构建 C++ 核心库与 `tests/` 四个测试项目，不含语言封装）。
- **Visual Studio**：Windows。

## 获取源码

```bash
git clone --recurse-submodules https://github.com/lailongwei/llbc.git
cd llbc
```

<div class="callout important" markdown="1">
**子模块**：语言封装与单元测试依赖三个 git 子模块——`wrap/pyllbc/cpython`、`wrap/lullbc/lua`
（Python/Lua 封装用）与 `tests/3rdparty/googletest`（`unit_test` 用）。
普通 clone 下这些目录**存在但为空**，会导致封装构建产出损坏的产物。务必带 `--recurse-submodules`，
或在已有仓库中执行：

```bash
git submodule update --init --recursive
```
</div>

## 路径一：premake → make（Linux / macOS，首选）

`Makefile` 会先调用 `tools/premake/premake5_<os>_<arch>` 生成 `build/gmake2/*.make`，
再分派到对应目标。产物落在 `output/gmake2/<config>/`。

`config` 默认 `release64`，可选值：`debug32`、`release32`、`debug64`、`release64`。

```bash
make help                          # 查看完整目标列表
make core_lib                      # 只编译 C++ 核心库（libllbc[.so|.dylib]）
make all                           # 核心库 + 全部测试 + 全部封装
make tests                         # 四个测试项目（example func_test unit_test quick_start）

# 常用：带 config 与并行
make core_lib config=debug64 -j4
make quick_start config=debug64    # 核心库 + 交互式 quick_start 演示
make unit_test  config=debug64     # 核心库 + gtest 单元测试
```

清理与安装：

```bash
make clean                         # 全量清理
make clean_core_lib                # 按目标清理（clean_tests / clean_func_test / ...）
make install                       # 安装核心库与头文件到 /usr/lib + /usr/include（需 sudo）
```

## 路径二：CMake（Linux / macOS，次选）

构建 C++ 核心库（`llbc_lib` 静态库 + `llbc_lib_shared`）与四个 `tests/` 项目，不含语言封装。
产物落在 `output/cmake/`。

```bash
mkdir cmake_build && cd cmake_build
cmake ..
make -j4
```

## 路径三：Visual Studio（Windows）

1. 运行仓库根目录的 `WinPreBuild.bat`。

   ![运行 WinPreBuild.bat](../assets/img/windows_prebuild_screenshot.png)

2. 选择一个 Visual Studio 版本（vs2005 … vs2022）。脚本会调用
   `tools/premake/premake5_windows.exe` 生成 `build/<vsXXXX>/llbc_<vsXXXX>.sln`。

   ![选择 VS 版本](../assets/img/llbc_win_configuration_choice_screenshot.png)

3. 用 Visual Studio 打开生成的解决方案，选择配置后编译。

   ![VS 项目列表](../assets/img/llbc_win_vs_projectlist_screenshot.png)

## 路径四：Xcode（iOS）

iOS 使用预生成的工程，位于 `iOSBuild/`。

## 语言封装

在核心库之上，可分别构建 Python / C# / Lua 封装（依赖前述子模块）：

```bash
make wraps                         # 全部封装
make py_wrap                       # Python（pyllbc）
make cs_wrap                       # C#（csllbc）
make lu_wrap                       # Lua（lullbc）
```

## 下一步

核心库编译成功后，前往 [Hello World](hello-world.md) 写下你的第一个 llbc 程序。
