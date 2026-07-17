#!/usr/bin/env bash
# ============================================================================
# llbc unit-test coverage runner (clang source-based coverage).
#
# Builds the unit_test target with coverage instrumentation, runs it, and emits
# a coverage report for the "modules under test".
#
# The set of reported source files is sensed DIRECTLY from the unit test sources:
# each test file declares the file(s) it exercises with co-located marker comments
#   // @coverage-target: <path relative to repo root>   (globs allowed)
# and this script collects every such marker under tests/unit_test/. No separate
# manifest to maintain -- add a test, add its marker in the same file.
#
# Policy: REPORT-ONLY. Coverage numbers never fail this script. It exits non-zero
# ONLY when configure/build fails, or when the test binary itself fails (so real
# regressions still surface) -- and even then the report is produced first.
#
# Env overrides:
#   CC / CXX                  compilers (default: clang / clang++)
#   LLVM_COV / LLVM_PROFDATA   llvm tools (default: llvm-cov / llvm-profdata)
#   UNIT_TEST_GTEST_FILTER    value for --gtest_filter (default: run all)
#   COVERAGE_JOBS             parallel build jobs (default: nproc/sysctl)
# ============================================================================
set -uo pipefail

# --- Locate repo root (this script lives in tools/coverage/). --------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "${ROOT}"

CC="${CC:-clang}"
CXX="${CXX:-clang++}"
LLVM_COV="${LLVM_COV:-llvm-cov}"
LLVM_PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
UNIT_TEST_DIR="${ROOT}/tests/unit_test"

BUILD_DIR="${ROOT}/output/coverage_build"
BIN_DIR="${ROOT}/output/cmake"
PROFRAW="${ROOT}/output/unit_test.profraw"
PROFDATA="${ROOT}/output/unit_test.profdata"
HTML_DIR="${ROOT}/output/coverage_html"
LCOV_FILE="${ROOT}/output/coverage.lcov"

if command -v nproc >/dev/null 2>&1; then JOBS="${COVERAGE_JOBS:-$(nproc)}";
elif command -v sysctl >/dev/null 2>&1; then JOBS="${COVERAGE_JOBS:-$(sysctl -n hw.ncpu)}";
else JOBS="${COVERAGE_JOBS:-4}"; fi

echo "==> repo root      : ${ROOT}"
echo "==> compiler       : ${CC} / ${CXX}"
echo "==> llvm tools     : ${LLVM_COV} / ${LLVM_PROFDATA}"
echo "==> build jobs     : ${JOBS}"

# --- 1. Configure (coverage-instrumented Debug build). ---------------------
echo "==> Configuring coverage build..."
cmake -S "${ROOT}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DLLBC_ENABLE_COVERAGE=ON \
    -DCMAKE_C_COMPILER="${CC}" \
    -DCMAKE_CXX_COMPILER="${CXX}" || { echo "!! cmake configure failed"; exit 1; }

# --- 2. Build unit_test. ---------------------------------------------------
# CMake target is llbc_unit_test (project name); its output binary is unit_test[_debug].
echo "==> Building unit_test..."
cmake --build "${BUILD_DIR}" --target llbc_unit_test -j "${JOBS}" || { echo "!! build failed"; exit 1; }

# The DEBUG build carries the _debug suffix (CMAKE_DEBUG_POSTFIX).
UNIT_TEST_BIN="${BIN_DIR}/unit_test_debug"
[ -x "${UNIT_TEST_BIN}" ] || UNIT_TEST_BIN="${BIN_DIR}/unit_test"
if [ ! -x "${UNIT_TEST_BIN}" ]; then
    echo "!! unit_test binary not found under ${BIN_DIR}"; exit 1
fi
echo "==> unit_test bin  : ${UNIT_TEST_BIN}"

# --- 3. Run the tests (records profile). -----------------------------------
rm -f "${PROFRAW}" "${PROFDATA}"
GTEST_ARGS=()
[ -n "${UNIT_TEST_GTEST_FILTER:-}" ] && GTEST_ARGS+=("--gtest_filter=${UNIT_TEST_GTEST_FILTER}")
echo "==> Running unit_test ${GTEST_ARGS[*]:-}"
# ${arr[@]+"${arr[@]}"} safely expands a possibly-empty array under `set -u` (bash 3.2 compat).
LLVM_PROFILE_FILE="${PROFRAW}" "${UNIT_TEST_BIN}" ${GTEST_ARGS[@]+"${GTEST_ARGS[@]}"}
TEST_RC=$?
[ "${TEST_RC}" -eq 0 ] && echo "==> tests passed" || echo "!! tests exited with code ${TEST_RC} (report still generated below)"

# --- 4. Merge raw profile. -------------------------------------------------
if [ ! -f "${PROFRAW}" ]; then
    echo "!! no profile produced (${PROFRAW}); cannot report coverage"; exit "${TEST_RC}"
fi
"${LLVM_PROFDATA}" merge -sparse "${PROFRAW}" -o "${PROFDATA}" || { echo "!! llvm-profdata merge failed"; exit 1; }

# --- 5. Collect @coverage-target markers from the unit test sources. -------
#     Sensed directly from tests/unit_test/**/*.cpp; expand globs; keep existing.
TARGETS=()
declare -a RAW_TARGETS=()
while IFS= read -r rel; do
    rel="$(echo "${rel}" | xargs 2>/dev/null || true)"   # trim
    [ -z "${rel}" ] && continue
    RAW_TARGETS+=("${rel}")
done < <(grep -rhoE '@coverage-target:[[:space:]]*[^[:space:]]+' "${UNIT_TEST_DIR}" \
             --include='*.cpp' --include='*.h' 2>/dev/null \
         | sed -E 's/.*@coverage-target:[[:space:]]*//' | sort -u)

if [ "${#RAW_TARGETS[@]}" -eq 0 ]; then
    echo "!! no @coverage-target markers found under ${UNIT_TEST_DIR}"; exit 1
fi

for rel in "${RAW_TARGETS[@]}"; do
    matched=0
    for path in ${ROOT}/${rel}; do        # glob expansion
        if [ -e "${path}" ]; then TARGETS+=("${path}"); matched=1; fi
    done
    [ "${matched}" -eq 0 ] && echo "!! @coverage-target path not found: ${rel}"
done

if [ "${#TARGETS[@]}" -eq 0 ]; then
    echo "!! no coverage target files resolved from @coverage-target markers"; exit 1
fi
echo "==> coverage targets (${#TARGETS[@]}) sensed from @coverage-target markers"

# --- 6. Report (tested modules only). --------------------------------------
echo ""
echo "================ Unit-test coverage (tested modules) ================"
REPORT="$("${LLVM_COV}" report "${UNIT_TEST_BIN}" -instr-profile="${PROFDATA}" "${TARGETS[@]}" 2>/dev/null)"
echo "${REPORT}"

# GitHub Actions job summary (markdown).
if [ -n "${GITHUB_STEP_SUMMARY:-}" ]; then
    {
        echo "## Unit-test coverage (tested modules)"
        [ "${TEST_RC}" -ne 0 ] && echo ":warning: unit_test exited with code ${TEST_RC}."
        echo '```'
        echo "${REPORT}"
        echo '```'
        echo "_Report-only: coverage numbers do not gate CI. Targets are sensed from \`@coverage-target:\` markers in tests/unit_test/._"
    } >> "${GITHUB_STEP_SUMMARY}"
fi

# --- 7. Export HTML + lcov artifacts. --------------------------------------
rm -rf "${HTML_DIR}"
"${LLVM_COV}" show "${UNIT_TEST_BIN}" -instr-profile="${PROFDATA}" \
    -format=html -output-dir="${HTML_DIR}" \
    -show-line-counts-or-regions "${TARGETS[@]}" >/dev/null 2>&1 \
    && echo "==> HTML report  : ${HTML_DIR}/index.html" || echo "!! HTML export failed (non-fatal)"
"${LLVM_COV}" export "${UNIT_TEST_BIN}" -instr-profile="${PROFDATA}" \
    -format=lcov "${TARGETS[@]}" > "${LCOV_FILE}" 2>/dev/null \
    && echo "==> lcov report  : ${LCOV_FILE}" || echo "!! lcov export failed (non-fatal)"

# Report-only: surface a genuine test failure via exit code, but never gate on coverage.
exit "${TEST_RC}"
