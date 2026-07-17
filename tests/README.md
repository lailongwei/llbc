# llbc 框架核心库(llbc) 测试项目 集合
## 1、简介
为标准化、自动化 及 全面性测试 llbc 核心库，建立此测试项目集合，计划分为以下几种类型的测试：
- `func_test`: 功能测试项目，历史遗留项目，历史上一直用于: 功能测试/单元测试/压力测试/...，随不同更具体意图的测试项目建立，此项目不会废弃，更多是**给框架开发者提供一种快速编写测试代码及快速验证的实现，并用于框架开发者内部交流**。
- `unit_test`: 单元测试项目，支持自化测试；
- `quick_start`: 快速开始项目，用于给开发者快速上手、学习的项目；
- `example`: 示例项目，每种特性都将有[1，n)个示例给到开发者参考及学习；

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

## 4、quick_start（快速开始）
一个快速开始项目，结合 wiki、通过几个典型功能的介绍及使用，让开发者能最快速度熟悉及了解 llbc。

## 5、example（示例）
示例项目，旨在为 llbc 框架每个功能提供 [1，n) 个测试用例，在开发者需要的时候，结合 wiki、通过此项目找到具体功能详细使用指南及最佳实践。
