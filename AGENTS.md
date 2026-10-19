# llbc repository guidance

## Scope and sources of truth

These instructions apply to the whole repository. A nested `AGENTS.md` may add
more specific rules for its directory.

- Treat the current source, `README.md`, `tests/README.md`, `CHANGELOG`, and the
  CMake files as authoritative. Do not rely on stale generated build files.
- llbc is a cross-platform C++17 server framework. Linux and Windows are the
  primary production platforms; macOS is supported but its networking path has
  lower production priority. iOS and Android are best-effort targets.
- The core library is under `llbc/`; language bindings are under `wrap/`; tests
  are under `tests/`; build and coverage helpers are under `tools/`.

## Working agreement

- Preserve unrelated work in a dirty worktree. Do not discard, overwrite, or
  reformat user changes outside the requested scope.
- Make the smallest coherent change that satisfies the requested behavior.
- Do not edit generated outputs under `build*`, `output/`, `cmake-build-*`, or
  `docs/_site` as source changes.
- Do not modify vendored submodule contents unless the task explicitly targets
  that dependency. The submodules are `tests/3rdparty/googletest`,
  `wrap/pyllbc/cpython`, and `wrap/lullbc/lua`.
- Do not add a production dependency without explaining its portability,
  maintenance, licensing, and ABI impact.

## Architecture

The public umbrella header includes modules in dependency order:

1. `common`: platforms, compilers, types, macros, errors, and streams.
2. `core`: threading, time, files, logging, configuration, object pools, and
   other non-networking facilities.
3. `comm`: services, components, sessions, packets, protocol stacks, and
   platform pollers.
4. `testcase`: base support for interactive examples and functional tests.
5. `app`: application lifecycle and multi-service composition.

`LLBC_Startup()` initializes these modules in order and `LLBC_Cleanup()` tears
them down. Public code generally assumes that lifecycle has been established.

## Build and test

Initialize only the submodules required by the task. For all supported targets:

```bash
git submodule update --init --recursive
```

Use CMake as the portable verification path:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
ctest --test-dir build --build-config Release --output-on-failure --no-tests=error
```

The Makefile is a convenience front end over CMake:

```bash
make llbc config=Release
make unit_test config=Release
make tests config=Release
```

Run a focused unit-test selection after building when that is sufficient for an
iteration, but run the complete CTest gate before declaring a C++ change done:

```bash
./output/Release/unit_test --gtest_filter='SuiteName.TestName'
```

For coverage-sensitive work:

```bash
CC=clang CXX=clang++ bash tools/coverage/run_unit_test_coverage.sh
```

CI is the authority for the configured Linux, macOS, Windows, and coverage
workflows. Do not assume that an unconfigured compiler or platform is covered.

## Change-specific gates

- Internal C++ change: build the affected targets and run the complete unit-test
  CTest gate.
- Public C++ API change: also search `wrap/pyllbc`, `wrap/csllbc`, and
  `wrap/lullbc` for bindings, update affected bindings and documentation, and
  build every locally supported affected wrapper.
- Threading, networking, filesystem, or process change: cover failure,
  cancellation/cleanup, and platform-specific paths. Avoid timing-only tests.
- CMake, coverage, or CI change: validate both single-config and multi-config
  assumptions and keep shell/PowerShell behavior aligned where applicable.
- Documentation-only change: verify referenced commands and paths against the
  current tree.

## C++ contracts

- Keep the project at strict C++17 unless the task explicitly changes the
  supported standard.
- Use `__LLBC_NS_BEGIN` / `__LLBC_NS_END` for public namespace scope and
  `__LLBC_INTERNAL_NS_BEGIN` / `__LLBC_INTERNAL_NS_END` for internals.
- Prefix public symbols with `LLBC_`; keep internal symbols out of public
  headers. Export new public API with `LLBC_EXPORT` when required.
- Follow the established `LLBC_OK` / `LLBC_FAILED` contract and set useful
  failure detail through the llbc last-error APIs.
- Put template or inline-heavy implementations in the sibling `*Inl.h` and
  include it from the public header.
- Pair acquired resources and lifecycle operations on every exit path. Review
  ownership, thread-safety, and shutdown order explicitly.
- Preserve public ABI unless the task explicitly authorizes a break.

## Tests

- Prefer an automated gtest under `tests/unit_test` for new behavior or a bug
  fix. Use `tests/func_test` only for interactive, stress, or developer flows.
- Add one or more `// @coverage-target:` comments naming the production source
  files exercised by each new unit-test file.
- The unit-test executable already owns `LLBC_Startup()` / `LLBC_Cleanup()` via
  its global environment; individual tests must not call them again.
- Keep tests deterministic and independent of execution order, wall-clock race
  margins, the developer's locale, and machine-specific absolute paths.
- Test both success and failure behavior, including last-error semantics and
  cleanup after partial initialization.

## Completion evidence

Before reporting completion:

1. Inspect the final diff and run `git diff --check`.
2. Run the narrowest relevant checks during iteration and the full required gate
   for the changed scope.
3. Report the exact checks run, their result, and any platform or dependency gate
   that could not be run locally.
