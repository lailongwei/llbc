---
name: llbc-change-workflow
description: Implement or review changes to the llbc C++ core, wrappers, tests, build system, or CI with repository-specific scope analysis and verification. Use for llbc feature work, bug fixes, API changes, test additions, and build or coverage maintenance; do not use for general C++ work outside this repository.
---

# llbc change workflow

Read the applicable `AGENTS.md` chain and inspect the current implementation,
tests, and build wiring before editing. Treat the current tree as authoritative.

## Classify the change

Identify every affected lane before implementation:

- Core implementation or private header.
- Public C++ API or ABI.
- Python, C#, or Lua wrapper surface.
- Automated unit test, interactive functional test, or example.
- CMake, platform toolchain, coverage, or GitHub Actions behavior.
- Documentation only.

Do not infer permission to change an unrelated lane. Public API work is the main
exception: it requires checking all three wrapper trees and user documentation
for the changed symbol.

## Implement with llbc invariants

- Preserve strict C++17 and the established namespace, export, naming, error,
  inline-header, and lifecycle conventions in the root `AGENTS.md`.
- For thread, service, session, socket, timer, process, and filesystem work,
  reason explicitly about ownership, partial initialization, cancellation,
  concurrent shutdown, and every error exit.
- Add or update a deterministic gtest for observable behavior. New unit-test
  files must name their production sources with `// @coverage-target:` markers.
- Keep generated outputs and dependency submodules out of the patch unless they
  are the explicit subject of the request.

## Choose verification from the change scope

Use focused checks while iterating, then run the complete applicable gate.

For normal C++ changes:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
ctest --test-dir build --build-config Release --output-on-failure --no-tests=error
```

For a focused test iteration:

```bash
./output/Release/unit_test --gtest_filter='SuiteName.TestName'
```

For coverage work:

```bash
CC=clang CXX=clang++ bash tools/coverage/run_unit_test_coverage.sh
```

For public API changes, search and build the affected bindings under
`wrap/pyllbc`, `wrap/csllbc`, and `wrap/lullbc`. State clearly when a platform or
toolchain prevents a local wrapper or CI lane from being exercised.

For CI or build changes, inspect the full workflow or CMake path rather than
validating only one extracted command. Check shell and PowerShell counterparts
when behavior is intended to match across platforms.

## Finish with evidence

Run `git diff --check`, inspect the complete final diff, and report:

- Changed behavior and files.
- Tests and validators actually run, with results.
- Unrun platform, wrapper, submodule, or secret-dependent checks.
- Remaining risks, if any.

Do not claim the full cross-platform CI passed from a single local build.
