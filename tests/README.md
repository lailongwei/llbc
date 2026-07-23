# llbc 框架核心库(llbc) 测试项目 集合
## 1、简介
为标准化、自动化 及 全面性测试 llbc 核心库，建立此测试项目集合，计划分为以下几种类型的测试：
- `func_test`: 功能测试项目，历史遗留项目，历史上一直用于: 功能测试/单元测试/压力测试/...，随不同更具体意图的测试项目建立，此项目不会废弃，更多是**给框架开发者提供一种快速编写测试代码及快速验证的实现，并用于框架开发者内部交流**。
- `unit_test`: 单元测试项目，支持自化测试；
- `example`: 示例/上手项目，用于给开发者快速上手、学习，并为 llbc 框架每种特性提供[1，n)个可跑示例给到开发者参考及学习；

*Tps: 所有测试项目依赖的三方依赖(包括`gtest/gmock`)，均放于 `tests/3rdparty` 目录。*

## 2、func_test（功能测试）
llbc 框架从 2011 年开始编写，到2026年，均依赖且只依赖此测试项目完成：单元测试/功能测试/压力测试/... 及其它所有测试，**随着 项目参与人越来越多 及 项目越来越庞大，仅依赖此无法全面、无法自动化的测试项目来确保框架整体质量的方案，无法持续（或可认为已经不可能再通过此简单项目来达成可持续性确保框架质量、稳定性及性能）**，此项目从 2026 年开始，将慢慢只作为功能测试用，且仅限于框架开发者内部人员使用，其它职责由其它项目代替。

## 3、unit_test（单元测试）
llbc 项目单元测试项目，使用 [gtest/gmock](https://github.com/google/googletest) 框架进行编码，gtest/gmock 版本: [v1.17.0](https://github.com/google/googletest/tree/v1.17.0)（发布于 May/1，2025），**llbc 框架的整体质量将慢慢通过此项目来确保**。

### 3.1、覆盖率（coverage）
每次 push / PR 都会由 GitHub Action `unit-test-coverage`（`.github/workflows/unit-test-coverage.yml`）自动测量单元测试覆盖率。基于 clang source-based coverage（`llvm-cov`），**只报告、不阻断**——覆盖率数字不会让 action 失败（编译失败、测试失败仍会失败），报告写入 job summary，HTML / lcov 作为 artifact 上传。

- **统计范围**：只统计「已有单测的模块」。被测源文件由**测试文件内的标记注释直接感知**，无需单独清单——脚本会扫描 `tests/unit_test/**` 下所有形如下面的标记并汇总：
  ```cpp
  // @coverage-target: llbc/src/core/utils/Util_Base64.cpp
  // @coverage-target: llbc/include/llbc/core/utils/Util_Base64Inl.h
  ```
- **约定**：**每新增一个单测，在该测试文件里就近加上它覆盖的源文件 `// @coverage-target:` 标记**（`llbc/src/**` 的 `.cpp`，以及 inline 较多的模块的 `*Inl.h`）；目标是把被测模块驱动到尽量接近 100%（region / function / line / branch）。
- **本地运行**（需要 `clang`/`clang++` 与配套 `llvm-cov`/`llvm-profdata`、`cmake`）：
  ```bash
  CC=clang CXX=clang++ bash tools/coverage/run_unit_test_coverage.sh
  # macOS 上 llvm 工具常不在 PATH，可显式指定：
  # LLVM_COV=$(brew --prefix llvm)/bin/llvm-cov LLVM_PROFDATA=$(brew --prefix llvm)/bin/llvm-profdata \
  #   CC=clang CXX=clang++ bash tools/coverage/run_unit_test_coverage.sh
  ```
  产物：控制台覆盖率表 + `output/coverage_html/index.html` + `output/coverage.lcov`。

## 4、example（示例 / 快速上手）
入门 + 各特性用法示例项目：一个菜单驱动的交互式程序（保留 `func_test` 同款菜单 + traits 宏系统），
既让开发者最快速度熟悉 llbc，也为每种特性提供[1，n)个最短可跑示例。

- **组织**：按模块分组 `common/ core/ comm/ app/`，文件与类名 `Example_<Module>_<Topic>`（如 `core/Example_Core_Stream`）。
- **注册**：在 `ExampleDef.h` 用 `__DEFINE_EXAMPLE("<菜单名>", <类名>)` 注册；**注册行必须连续**（id 依赖 `__LINE__`，勿在 `__EXAMPLES_BEGIN()`/`__EXAMPLES_END()` 间插空行或整行注释），分组用排序 + 行尾注释表达。
- **约定**：每个示例继承 `LLBC_BaseTestCase` 只重写 `Run()`；**不要**在 `Run()` 内调用 `LLBC_Startup()/LLBC_Cleanup()`——框架由菜单主程序 `Example.cpp` 统一启停（`LLBC_Startup` 不可重入）。
- **构建运行**：`make example config=debug64` → `output/gmake2/debug64/example_debug`；与文档站点一一对应，见 `docs/examples.md`。
