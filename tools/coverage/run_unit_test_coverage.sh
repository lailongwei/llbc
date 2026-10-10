#!/usr/bin/env bash
# ============================================================================
# llbc unit-test coverage runner.
#
# Clang/AppleClang use source-based coverage (llvm-cov); GCC uses gcov/gcovr.
# By default this script configures, builds, and runs the unit tests. Pass
# --report-only to reuse an already-built and already-tested CI build.
#
# Reported sources are sensed directly from tests/unit_test/ marker comments:
#   // @coverage-target: <path relative to repo root>   (globs allowed)
#
# Coverage percentages are report-only. Configuration, build, test, profile
# processing, or report-generation failures still fail the script.
#
# Env overrides:
#   CC / CXX                  compilers (default: clang / clang++)
#   COVERAGE_BACKEND          llvm or gcov (auto-detected from CXX)
#   LLVM_COV / LLVM_PROFDATA  LLVM tools (default: llvm-cov / llvm-profdata)
#   GCOV / GCOVR              GNU coverage tools (default: gcov / gcovr)
#   UNIT_TEST_GTEST_FILTER     GoogleTest filter (default: run all)
#   COVERAGE_JOBS              parallel build jobs (default: nproc/sysctl)
#   COVERAGE_BUILD_DIR         CMake build directory
#   COVERAGE_CONFIGURATION     CMake configuration (default: Debug)
#   COVERAGE_UNIT_TEST_BIN     instrumented static unit-test executable (LLVM)
#   COVERAGE_PLATFORM          display platform for the normalized summary
#   COVERAGE_COMPILER          display compiler for the normalized summary
#   COVERAGE_SUMMARY_FILE      normalized per-file summary output path
# ============================================================================
set -uo pipefail

MODE="${1:-run}"
if [ "${MODE}" != "run" ] && [ "${MODE}" != "--report-only" ]; then
    echo "Usage: $0 [--report-only]"
    exit 2
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "${ROOT}" || exit 1

CC="${CC:-clang}"
CXX="${CXX:-clang++}"
LLVM_COV="${LLVM_COV:-llvm-cov}"
LLVM_PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
GCOV="${GCOV:-gcov}"
GCOVR="${GCOVR:-gcovr}"
UNIT_TEST_DIR="${ROOT}/tests/unit_test"

BACKEND="${COVERAGE_BACKEND:-}"
if [ -z "${BACKEND}" ]; then
    if "${CXX}" --version 2>/dev/null | grep -qi clang; then
        BACKEND=llvm
    else
        BACKEND=gcov
    fi
fi
if [ "${BACKEND}" != "llvm" ] && [ "${BACKEND}" != "gcov" ]; then
    echo "!! unsupported coverage backend: ${BACKEND}"
    exit 2
fi

BUILD_DIR="${COVERAGE_BUILD_DIR:-${ROOT}/output/coverage_build}"
CONFIGURATION="${COVERAGE_CONFIGURATION:-Debug}"
OUTPUT_DIR="${ROOT}/output"
PROFRAW_PATTERN="${OUTPUT_DIR}/unit_test-%p.profraw"
PROFDATA="${OUTPUT_DIR}/unit_test.profdata"
LCOV_FILE="${OUTPUT_DIR}/coverage.lcov"
SUMMARY_FILE="${COVERAGE_SUMMARY_FILE:-${OUTPUT_DIR}/coverage-summary.json}"
PYTHON="${PYTHON:-python3}"

if command -v nproc >/dev/null 2>&1; then JOBS="${COVERAGE_JOBS:-$(nproc)}";
elif command -v sysctl >/dev/null 2>&1; then JOBS="${COVERAGE_JOBS:-$(sysctl -n hw.ncpu)}";
else JOBS="${COVERAGE_JOBS:-4}"; fi

echo "==> repo root      : ${ROOT}"
echo "==> compiler       : ${CC} / ${CXX}"
echo "==> coverage       : ${BACKEND}"
echo "==> configuration  : ${CONFIGURATION}"
echo "==> build jobs     : ${JOBS}"

TEST_RC=0
if [ "${MODE}" = "run" ]; then
    echo "==> Configuring coverage build..."
    cmake -S "${ROOT}" -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE="${CONFIGURATION}" \
        -DLLBC_ENABLE_COVERAGE=ON \
        -DCMAKE_C_COMPILER="${CC}" \
        -DCMAKE_CXX_COMPILER="${CXX}" || { echo "!! cmake configure failed"; exit 1; }

    echo "==> Building unit tests..."
    cmake --build "${BUILD_DIR}" \
        --target unit_test_static unit_test_shared \
        --parallel "${JOBS}" || { echo "!! build failed"; exit 1; }

    rm -f "${OUTPUT_DIR}"/unit_test-*.profraw "${PROFDATA}"
    find "${BUILD_DIR}" -type f -name '*.gcda' -delete
    echo "==> Running unit tests through CTest..."
    if [ -n "${UNIT_TEST_GTEST_FILTER:-}" ]; then
        export GTEST_FILTER="${UNIT_TEST_GTEST_FILTER}"
    fi
    if [ "${BACKEND}" = "llvm" ]; then
        LLVM_PROFILE_FILE="${PROFRAW_PATTERN}" \
            ctest --test-dir "${BUILD_DIR}" --build-config "${CONFIGURATION}" \
                  --output-on-failure --no-tests=error
    else
        ctest --test-dir "${BUILD_DIR}" --build-config "${CONFIGURATION}" \
              --output-on-failure --no-tests=error
    fi
    TEST_RC=$?
    [ "${TEST_RC}" -eq 0 ] && echo "==> tests passed" || \
        echo "!! tests exited with code ${TEST_RC} (report still generated below)"
fi

# Collect and expand co-located @coverage-target markers.
TARGETS=()
declare -a RAW_TARGETS=()
while IFS= read -r rel; do
    rel="$(echo "${rel}" | xargs 2>/dev/null || true)"
    [ -z "${rel}" ] && continue
    RAW_TARGETS+=("${rel}")
done < <(grep -rhoE '@coverage-target:[[:space:]]*[^[:space:]]+' "${UNIT_TEST_DIR}" \
             --include='*.cpp' --include='*.h' 2>/dev/null \
         | sed -E 's/.*@coverage-target:[[:space:]]*//' | sort -u)

if [ "${#RAW_TARGETS[@]}" -eq 0 ]; then
    echo "!! no @coverage-target markers found under ${UNIT_TEST_DIR}"
    exit 1
fi

for rel in "${RAW_TARGETS[@]}"; do
    matched=0
    for path in ${ROOT}/${rel}; do
        if [ -e "${path}" ]; then TARGETS+=("${path}"); matched=1; fi
    done
    [ "${matched}" -eq 0 ] && echo "!! @coverage-target path not found: ${rel}"
done

if [ "${#TARGETS[@]}" -eq 0 ]; then
    echo "!! no coverage target files resolved from @coverage-target markers"
    exit 1
fi
echo "==> coverage targets (${#TARGETS[@]}) sensed from @coverage-target markers"

mkdir -p "${OUTPUT_DIR}"

if [ "${BACKEND}" = "llvm" ]; then
    UNIT_TEST_BIN="${COVERAGE_UNIT_TEST_BIN:-}"
    if [ -z "${UNIT_TEST_BIN}" ]; then
        for candidate in \
            "${OUTPUT_DIR}/${CONFIGURATION}/unit_test_static_debug" \
            "${OUTPUT_DIR}/${CONFIGURATION}/unit_test_static" \
            "${OUTPUT_DIR}/cmake/unit_test_static_debug" \
            "${OUTPUT_DIR}/cmake/Debug/unit_test_static_debug" \
            "${OUTPUT_DIR}/cmake/unit_test_static"; do
            if [ -x "${candidate}" ]; then
                UNIT_TEST_BIN="${candidate}"
                break
            fi
        done
    fi
    if [ ! -x "${UNIT_TEST_BIN}" ]; then
        echo "!! unit_test binary not found under ${OUTPUT_DIR}"
        exit 1
    fi
    echo "==> unit_test bin  : ${UNIT_TEST_BIN}"

    shopt -s nullglob
    PROFRAW_FILES=("${OUTPUT_DIR}"/unit_test-*.profraw)
    shopt -u nullglob
    if [ "${#PROFRAW_FILES[@]}" -eq 0 ]; then
        echo "!! no profiles produced (${PROFRAW_PATTERN}); cannot report coverage"
        [ "${TEST_RC}" -ne 0 ] && exit "${TEST_RC}"
        exit 1
    fi
    "${LLVM_PROFDATA}" merge -sparse "${PROFRAW_FILES[@]}" -o "${PROFDATA}" || \
        { echo "!! llvm-profdata merge failed"; exit 1; }

    REPORT="$("${LLVM_COV}" report "${UNIT_TEST_BIN}" \
        -instr-profile="${PROFDATA}" "${TARGETS[@]}" 2>/dev/null)" || \
        { echo "!! coverage summary generation failed"; exit 1; }

    "${LLVM_COV}" export "${UNIT_TEST_BIN}" -instr-profile="${PROFDATA}" \
        -format=lcov "${TARGETS[@]}" > "${LCOV_FILE}" 2>/dev/null || \
        { echo "!! lcov export failed"; exit 1; }
else
    GCOVR_ARGS=(
        --root "${ROOT}"
        --gcov-executable "${GCOV}"
        # GCC can emit negative branch hits for exception-related branches.
        # Gcovr documents this as GCC bug 68080; retain the rest of the data.
        --gcov-ignore-parse-errors negative_hits.warn_once_per_file
    )
    for path in "${TARGETS[@]}"; do
        # Relative filters are stable even when the Actions workspace is
        # reached through a symlink and gcovr canonicalizes source paths.
        relative_path="${path#"${ROOT}/"}"
        escaped_path="$(printf '%s' "${relative_path}" | sed 's/[][\\.^$*+?(){}|]/\\&/g')"
        GCOVR_ARGS+=(--filter "^${escaped_path}$")
    done
    REPORT="$("${GCOVR}" "${GCOVR_ARGS[@]}" "${BUILD_DIR}" \
        --txt - \
        --lcov "${LCOV_FILE}")" || \
        { echo "!! gcovr report generation failed"; exit 1; }
fi

echo ""
echo "================ Unit-test coverage (tested modules) ================"
echo "${REPORT}"
echo "==> lcov report  : ${LCOV_FILE}"
"${PYTHON}" "${SCRIPT_DIR}/coverage_summary.py" summarize \
    --input "${LCOV_FILE}" \
    --format lcov \
    --source-root "${ROOT}" \
    --markers-dir "${UNIT_TEST_DIR}" \
    --platform "${COVERAGE_PLATFORM:-$(uname -s)}" \
    --compiler "${COVERAGE_COMPILER:-${CXX}}" \
    --backend "${BACKEND}" \
    --output "${SUMMARY_FILE}" || { echo "!! coverage summary normalization failed"; exit 1; }
echo "==> summary      : ${SUMMARY_FILE}"

exit "${TEST_RC}"
