# Unit-test guidance

These rules extend the repository-root `AGENTS.md` for `tests/unit_test`.

- Name files consistently with their module and subject, following nearby
  `UnitTest_<Module>_<Subject>.cpp` examples.
- Put `// @coverage-target: <repo-relative-production-path>` markers in every
  new test file and keep them accurate when production coverage moves.
- Do not call `LLBC_Startup()` or `LLBC_Cleanup()` inside a test. `UnitTest.cpp`
  establishes the process-wide lifecycle once through a gtest environment.
- Use fixtures only for genuinely shared setup. Each test must release its own
  threads, services, files, sockets, timers, and callbacks before returning.
- Prefer observable synchronization over sleeps. If a timeout is necessary, use
  it only as a bounded failure guard, not as the condition that makes a test pass.
- Use temporary paths and dynamically available ports. Do not depend on the
  repository location, local timezone/locale, network access, or test order.
- Exercise both static and shared linkage through CTest before completion:

```bash
ctest --test-dir build --build-config Release --output-on-failure \
  --no-tests=error -L unit
```
