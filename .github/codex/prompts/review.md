# llbc pull-request review

Review the pull-request diff against its base branch. This is a read-only review:
do not modify files, create commits, or push changes.

Read the applicable `AGENTS.md` files first. Report only concrete, actionable
problems introduced by this pull request. Prioritize correctness over style and
avoid speculative findings without a reproducible failure mode.

Focus on:

1. Memory safety, undefined behavior, resource ownership, and cleanup on partial
   initialization or failure paths.
2. Thread safety, races, deadlocks, callbacks after destruction, service/session
   shutdown ordering, and timer lifetime.
3. Cross-platform behavior across GCC, Clang, AppleClang, MSVC, and clang-cl,
   including filesystem, socket, process, and compiler-front-end differences.
4. Public API and ABI compatibility, llbc namespace/export/error conventions,
   and missing updates to Python, C#, or Lua wrappers.
5. Test quality: missing regression cases, nondeterministic timing, duplicated
   startup/cleanup, static/shared linkage differences, and inaccurate or missing
   `@coverage-target` markers.
6. Build, CI, and coverage changes that work in only part of the configured CI
   workflows or accidentally weaken an existing quality gate.

For each finding, provide a priority (`P0` through `P3`), a concise title, the
smallest relevant file and line range, the failure scenario, and a practical fix.
If there are no actionable findings, say so explicitly. Write the review in
Chinese while keeping identifiers and commands unchanged.
