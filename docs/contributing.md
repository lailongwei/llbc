---
layout: default
title: 贡献指南
---

# 贡献指南

本页面面向希望向 llbc 提交代码或文档的贡献者，涵盖环境搭建、构建路径选择、
测试要求、以及项目的命名与代码风格约定。在动笔之前请完整阅读本页——约定不
合规的 PR 通常需要来回修改。

## 克隆仓库

llbc 包含三个 git 子模块，**必须**在克隆时或首次构建前初始化，否则
包装层构建会产生静默的残缺产物，`unit_test` 也无法找到 googletest：

```bash
# 推荐：克隆时一次性拉取所有子模块
git clone --recurse-submodules https://github.com/lailongwei/llbc.git

# 已经克隆但忘记了子模块：
git submodule update --init --recursive
```

三个子模块及其用途：

| 路径 | 用途 |
|------|------|
| `wrap/pyllbc/cpython` | Python 包装层 (pyllbc) |
| `wrap/lullbc/lua` | Lua 包装层 (lullbc) |
| `tests/3rdparty/googletest` | unit_test 所需的 gtest/gmock v1.17.0 |

## 选择构建路径

项目支持三条构建路径；根据你的目标平台和需求选择：

### premake → make（Linux / macOS，主路径）

```bash
# config 默认 release64；可选 debug32 / release32 / debug64 / release64
make core_lib config=debug64        # 只编核心库
make unit_test config=debug64       # 核心库 + gtest 单元测试
make func_test config=debug64 -j4   # 核心库 + 交互式功能测试
make all                            # 核心库 + 全部测试 + 全部包装层
```

输出落在 `output/gmake2/<config>/` 下（例如 `unit_test`、`unit_test_debug`）。

### CMake（Linux / macOS，辅助路径）

```bash
mkdir cmake_build && cd cmake_build && cmake .. && make -j4
```

CMake **不支持**包装层（pyllbc / csllbc / lullbc），仅构建核心库和四个
`tests/` 项目。输出落在 `output/cmake/`。

### Visual Studio（Windows）

```bat
REM 先生成 .sln，再用 VS 打开或 msbuild
WinPreBuild.bat
REM 然后在 build/vsXXXX/ 下打开 llbc_vsXXXX.sln
```

`WinPreBuild.bat` 调用 `tools/premake/premake5_windows.exe`，支持 vs2005 至 vs2022。

<div class="callout note" markdown="1">
修改公开 C++ API 后，应同步检查三个包装层的绑定代码（`wrap/pyllbc/src`、
`wrap/csllbc/native/src`、`wrap/lullbc/src`），它们镜像了 C++ 接口，但只有实际
构建包装层时才会暴露编译错误。
</div>

## 测试：unit_test 是质量门

llbc 有四个测试项目；新代码的自动化验证依赖 **unit_test**：

| 项目 | 类型 | 说明 |
|------|------|------|
| `tests/unit_test` | **自动化**，gtest/gmock | 进行中的质量门，CI 必须全绿 |
| `tests/func_test` | 交互式，菜单驱动 | 框架开发者功能验证，用 `__DEFINE_TEST_CASE(...)` 注册用例 |
| `tests/example` | 交互式，菜单驱动 | 新手上手 + 各特性用法演示，按 `common/core/comm/app` 分组，用 `__DEFINE_EXAMPLE(...)` 在 `ExampleDef.h` 注册 |

**为新特性 / 修复提交 unit_test 用例是硬性要求。** 编写方式：

```cpp
// tests/unit_test/core/your_module/UnitTest_Core_YourModule.cpp
#include <llbc.h>
using namespace llbc;

#include <gtest/gtest.h>

TEST(YourModuleTest, BasicBehavior)
{
    // LLBC_Startup / LLBC_Cleanup 已由 TestEnv_LLBCIniter 统一处理，
    // 用例内直接使用框架 API
    ASSERT_EQ(LLBC_OK, 0);
}
```

`tests/unit_test/UnitTest.cpp` 中的 `TestEnv_LLBCIniter` 会在所有
用例运行前后自动调用 `LLBC_Startup()` / `LLBC_Cleanup()`，无需在
单个用例里重复处理。

运行与过滤：

```bash
./output/gmake2/debug64/unit_test                          # 全量运行
./output/gmake2/debug64/unit_test --gtest_filter=Stream*   # 只跑 Stream 相关
```

## 命名与命名空间约定

### 命名空间宏

公共类型**必须**用宏，不能裸写 `namespace llbc { }`：

```cpp
// 正确
__LLBC_NS_BEGIN
class LLBC_MyClass { /* ... */ };
__LLBC_NS_END

// 内部/私有符号
__LLBC_INTERNAL_NS_BEGIN
class __LLBCMyHelper { /* ... */ };
__LLBC_INTERNAL_NS_END
```

宏定义见 `llbc/include/llbc/common/Macro.h`。

### 符号前缀

| 符号类型 | 前缀 | 示例 |
|----------|------|------|
| 公开类型 / 函数 / 宏 | `LLBC_` | `LLBC_Service`、`LLBC_OK` |
| 内部 / 私有符号 | `__LLBC_` 或 `LLBC_INTERNAL_` | `__LLBC_NS_BEGIN` |

**公开头文件中不得出现 `__LLBC_` 前缀的实现细节。**

### 可见性标注

新增公开 API 需要显式标注导出属性：

```cpp
// 公开：Windows 导出 / Linux 默认可见
LLBC_EXPORT void LLBC_MyFunc();

// 隐藏：不进入动态库符号表
LLBC_HIDDEN void InternalHelper();
```

`LLBC_EXPORT` / `LLBC_HIDDEN` 来自 `llbc/include/llbc/common/Export.h`。

### 返回值与错误码

```cpp
// 多数 API 返回 LLBC_OK (0) 或 LLBC_FAILED (-1)
int result = LLBC_DoSomething();
if (result == LLBC_FAILED)
{
    const char *err = LLBC_FormatLastError();  // 读取错误描述
    // ...
}
```

**不要**自造 int 返回码；新错误值扩展 `llbc/include/llbc/common/Errors.h`，
错误描述通过 `LLBC_SetLastError()` / `LLBC_GetLastError()` 传递。

## `*Inl.h` 惯例

模板或大量内联实现放在与头文件同目录的 `*Inl.h` 文件中，并在主头文件末尾
`#include` 进来：

```
llbc/include/llbc/comm/Service.h      ← 公开接口声明
llbc/include/llbc/comm/ServiceInl.h   ← 模板/内联实现
```

```cpp
// Service.h 末尾
#include "llbc/comm/ServiceInl.h"
```

这样消费方只 `#include "Service.h"` 就能得到完整定义，同时保持主头文件
的可读性。**禁止把实现直接写进主头文件**（除非真的只有一两行 getter）。

## 提交与 PR 流程

1. **Fork → feature branch**：从 `master` 创建描述性分支（如 `feat/stream-read-line`、`fix/timer-cancel-race`）。
2. **保持原子性**：每个 commit 对应一个逻辑改动；commit message 用简体中文或英文均可，但要言之有物。
3. **测试全绿**：提交前在本机跑 `make unit_test && ./output/gmake2/release64/unit_test`，确认无失败用例。
4. **包装层同步**：修改公开 C++ API 时，同步更新三个包装层绑定代码并验证能够编译。
5. **文档更新**：新增公开 API 或行为变更时，在 `docs/` 对应页面补充说明，并在 `CHANGELOG` 顶部追加变更条目。

<div class="callout warning" markdown="1">
**v1.1.1 之后的陷阱**：若你在处理日志、组件生命周期、流、对象池、或时间
相关代码，先查阅 `CHANGELOG` 顶部条目——这些模块在 v1.1.1 均有不向后兼容
的重构。具体地：日志用 `consoleLogLevel` / `fileLogLevel`（废弃 `logLevel`）；
组件非流程事件走 `OnEvent()`（废弃 `OnAppConfigReloaded()`）；`LLBC_Stream`
读写游标独立；对象池只用 `ObjPool.h` / `ThreadSpecObjPool.h`（旧池已删除）。
</div>

## 参照

- 项目约定文档：`CLAUDE.md`（仓库根）
- 命名空间宏：`llbc/include/llbc/common/Macro.h`
- 导出属性：`llbc/include/llbc/common/Export.h`
- 错误码定义：`llbc/include/llbc/common/Errors.h`
- unit_test 入口：`tests/unit_test/UnitTest.cpp`
- unit_test 用例示例：`tests/unit_test/common/UnitTest_Com_Stream.cpp`
- unit_test 用例示例：`tests/unit_test/core/variant/UnitTest_Core_Variant.cpp`
- func_test 用例注册：`tests/func_test/FuncTestDef.h`

## 下一步

- 了解整体架构：[架构概览](concepts/architecture.md)
- 搭建第一个服务：[Hello World](getting-started/hello-world.md)
- 构建系统详解：[构建指南](getting-started/build.md)
