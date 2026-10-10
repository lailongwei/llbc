---
layout: default
title: 安装与构建
---

# 安装与构建

llbc 使用 **CMake** 统一支持 Linux、macOS 与 Windows。本页先教你把**核心库**编译出来，
后续的 [Hello World](hello-world.md) 会直接链接它。CMake 同时会加入 `tests/`，并在对应
依赖可用时加入 `wrap/` 下的语言封装。

## 获取源码

```bash
git clone --recurse-submodules https://github.com/lailongwei/llbc.git
cd llbc
```

<div class="callout important" markdown="1">
**子模块**：`unit_test` 依赖 git 子模块 `tests/3rdparty/googletest`。普通 clone 下该目录
**存在但为空**，CMake 在配置阶段检测不到它时会**跳过 `unit_test` 目标**（其余目标照常构建）。
如需构建单元测试，务必带 `--recurse-submodules`，或在已有仓库中执行：

```bash
git submodule update --init tests/3rdparty/googletest
```
</div>

## Linux / macOS

要求 CMake **≥ 3.16**。构建 C++ 核心库（`llbc_lib` 静态库 + `llbc_lib_shared` 动态库）
与三个 `tests/` 项目（`example`、`func_test`、`unit_test`）。

```bash
mkdir cmake_build && cd cmake_build
cmake ..
make -j4
```

产物统一落在 `output/cmake/`（平铺目录）：

- 核心库：`libllbc.a`（静态）、`libllbc.dylib` / `libllbc.so`（动态）。
- 测试程序：每个测试各产出静态链接与动态链接两个可执行文件，例如
  `unit_test_static` 与 `unit_test_shared`。

<div class="callout note" markdown="1">
默认构建类型为 **Release**。单配置生成器（Makefile / Ninja）下切换到 Debug 需在配置时指定：

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

Debug 目标带 `_debug` 后缀（如 `libllbc_debug.a`、`unit_test_static_debug`）。
</div>

可选开关（配置时以 `-D<OPTION>=ON` 传入，均定义于 `tools/cmake/config.cmake`）：

| 开关 | 作用 |
|------|------|
| `LLBC_ENABLE_ASAN` | 启用 AddressSanitizer（非 Windows）。 |
| `LLBC_ENABLE_COVERAGE` | 启用编译器原生覆盖率支持（GCC/gcov、Clang/llvm-cov、MSVC 前端/PDB）。 |
| `LLBC_DISABLE_CXX11_ABI` | 定义 `_GLIBCXX_USE_CXX11_ABI=0`。 |

## Windows（MSVC / clang-cl）

CI 使用 CMake 的 Visual Studio 生成器分别验证 MSVC 与 clang-cl：

```powershell
# MSVC
cmake -S . -B cmake_build -A x64

# clang-cl（使用 MSVC ABI、运行库与 Windows SDK）
cmake -S . -B cmake_build_clang -A x64 -T ClangCL

cmake --build cmake_build_clang --config Release --parallel 2
ctest --test-dir cmake_build_clang --build-config Release --output-on-failure
```

## 语言封装（Python / C# / Lua）

CMake 会在依赖和当前平台满足条件时自动加入 `pyllbc`、`csllbc` 与 `lullbc`；不满足条件的
封装会在配置阶段给出提示并跳过，不影响核心库与测试项目的构建。

<div class="callout note" markdown="1">
封装依赖对应子模块：`wrap/pyllbc/cpython`（Python）、`wrap/lullbc/lua`（Lua）。
普通 clone 下它们存在但为空，务必先 `git submodule update --init --recursive`。
</div>

## 下一步

核心库编译成功后，前往 [Hello World](hello-world.md) 写下你的第一个 llbc 程序。
