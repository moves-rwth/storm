#!/usr/bin/env bash
set -euo pipefail

# Benchmark Storm compile time across:
# - PCH enabled/disabled
# - ccache cold/warm
#
# The script uses ccache through CMake compiler launchers and controls PCH via
# CMAKE_DISABLE_PRECOMPILE_HEADERS, so all four combinations are measurable.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_ROOT="${SOURCE_DIR}/build-bench-pch-ccache"
CCACHE_ROOT=""
BUILD_TYPE="Release"
TARGET="storm"
JOBS="$(command -v nproc >/dev/null 2>&1 && nproc || echo 4)"
GENERATOR=""
EXTRA_CMAKE_ARGS=()

usage() {
    cat <<'EOF'
Usage: benchmark-pch-ccache.sh [options]

Options:
    --source-dir <path>     Storm source directory (default: repo root)
    --build-root <path>     Directory for benchmark build trees/results
    --ccache-root <path>    Directory for benchmark ccache data (default: <build-root>/ccache)
    --build-type <type>     CMake build type (default: Release)
    --target <target>       Build target (default: storm)
    --jobs <n>              Parallel build jobs (default: nproc or 4)
    --generator <name>      CMake generator (e.g. Ninja)
    --cmake-arg <arg>       Extra CMake argument (can be repeated)
    --help                  Show this help

Output:
  - Summary table in stdout
  - CSV at <build-root>/results.csv
  - Per-run ccache stats in <build-root>/results/
EOF
}

require_cmd() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "Error: required command '$1' not found" >&2
        exit 1
    fi
}

extract_stat() {
    local label="$1"
    local file="$2"
    awk -F: -v key="$label" '
        $1 ~ key {
            gsub(/^[[:space:]]+|[[:space:]]+$/, "", $2)
            gsub(/[[:space:]]/, "", $2)
            print $2
            found=1
            exit
        }
        END { if (!found) print 0 }
    ' "$file"
}

run_case() {
    local pch_mode="$1"      # on|off
    local cache_state="$2"   # cold|warm
    local namespace="storm-bench-pch-${pch_mode}"
    local ccache_dir="${CCACHE_ROOT}/${namespace}"
    local disable_pch="OFF"

    if [[ "$pch_mode" == "off" ]]; then
        disable_pch="ON"
    fi

    local build_dir="${BUILD_ROOT}/build-${pch_mode}"
    local result_dir="${BUILD_ROOT}/results"
    mkdir -p "$result_dir"

    export CCACHE_DIR="$ccache_dir"
    export CCACHE_NAMESPACE="$namespace"
    export CCACHE_BASEDIR="$SOURCE_DIR"
    export CCACHE_NOHASHDIR="1"

    if [[ "$cache_state" == "cold" ]]; then
        rm -rf "$ccache_dir"
        rm -rf "$build_dir"
    fi
    mkdir -p "$ccache_dir"
    ccache --zero-stats >/dev/null

    local cmake_cmd=(cmake -S "$SOURCE_DIR" -B "$build_dir"
        -DCMAKE_DISABLE_PRECOMPILE_HEADERS="$disable_pch"
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE")

    if [[ -n "$GENERATOR" ]]; then
        cmake_cmd+=(-G "$GENERATOR")
    fi

    if [[ ${#EXTRA_CMAKE_ARGS[@]} -gt 0 ]]; then
        cmake_cmd+=("${EXTRA_CMAKE_ARGS[@]}")
    fi

    if [[ "$cache_state" == "cold" || ! -d "$build_dir" ]]; then
        echo "[benchmark] Configure: pch=${pch_mode}, cache=${cache_state}"
        "${cmake_cmd[@]}" >/dev/null
    fi

    if [[ "$cache_state" == "warm" ]]; then
        # Rebuild in the same build directory to keep paths stable for ccache.
        cmake --build "$build_dir" --target clean -j "$JOBS" >/dev/null || true
    fi

    local time_file="${result_dir}/time-${pch_mode}-${cache_state}.txt"
    echo "[benchmark] Build:     pch=${pch_mode}, cache=${cache_state}"
    /usr/bin/time -f "%e" -o "$time_file" \
        cmake --build "$build_dir" --target "$TARGET" -j "$JOBS" >/dev/null

    local stats_file="${result_dir}/ccache-${pch_mode}-${cache_state}.txt"
    ccache --show-stats --verbose >"$stats_file"

    local elapsed
    elapsed="$(cat "$time_file")"

    local cacheable hits misses uncacheable
    cacheable="$(extract_stat "Cacheable calls" "$stats_file")"
    hits="$(extract_stat "Hits" "$stats_file")"
    misses="$(extract_stat "Misses" "$stats_file")"
    uncacheable="$(extract_stat "Uncacheable calls" "$stats_file")"

    local hit_rate="0.00"
    if [[ "$cacheable" != "0" ]]; then
        hit_rate="$(awk -v h="$hits" -v c="$cacheable" 'BEGIN { printf "%.2f", (100.0*h)/c }')"
    fi

    printf "%s,%s,%s,%s,%s,%s,%s\n" \
        "$pch_mode" "$cache_state" "$elapsed" "$cacheable" "$hits" "$misses" "$uncacheable" \
        >>"${BUILD_ROOT}/results.csv"

    printf "%-8s %-6s %10s %12s %10s %10s %12s %9s\n" \
        "$pch_mode" "$cache_state" "$elapsed" "$cacheable" "$hits" "$misses" "$uncacheable" "${hit_rate}%"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --source-dir)
            SOURCE_DIR="$2"
            shift 2
            ;;
        --build-root)
            BUILD_ROOT="$2"
            shift 2
            ;;
        --ccache-root)
            CCACHE_ROOT="$2"
            shift 2
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --target)
            TARGET="$2"
            shift 2
            ;;
        --jobs)
            JOBS="$2"
            shift 2
            ;;
        --generator)
            GENERATOR="$2"
            shift 2
            ;;
        --cmake-arg)
            EXTRA_CMAKE_ARGS+=("$2")
            shift 2
            ;;
        --help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage
            exit 1
            ;;
    esac
done

require_cmd cmake
require_cmd ccache
require_cmd /usr/bin/time

if [[ -z "$CCACHE_ROOT" ]]; then
    CCACHE_ROOT="${BUILD_ROOT}/ccache"
fi

mkdir -p "$BUILD_ROOT"
mkdir -p "$CCACHE_ROOT"
: >"${BUILD_ROOT}/results.csv"
echo "pch,cache_state,build_seconds,cacheable_calls,hits,misses,uncacheable_calls" >>"${BUILD_ROOT}/results.csv"

echo
echo "Benchmark settings"
echo "  source-dir:  $SOURCE_DIR"
echo "  build-root:  $BUILD_ROOT"
echo "  ccache-root: $CCACHE_ROOT"
echo "  build-type:  $BUILD_TYPE"
echo "  target:      $TARGET"
echo "  jobs:        $JOBS"
if [[ -n "$GENERATOR" ]]; then
    echo "  generator:   $GENERATOR"
fi
if [[ ${#EXTRA_CMAKE_ARGS[@]} -gt 0 ]]; then
    echo "  extra args:  ${EXTRA_CMAKE_ARGS[*]}"
fi

echo
echo "Results"
printf "%-8s %-6s %10s %12s %10s %10s %12s %9s\n" \
    "pch" "cache" "seconds" "cacheable" "hits" "misses" "uncacheable" "hit-rate"
printf "%-8s %-6s %10s %12s %10s %10s %12s %9s\n" \
    "--------" "------" "----------" "------------" "----------" "----------" "------------" "---------"

run_case on cold
run_case on warm
run_case off cold
run_case off warm

echo
echo "CSV written to: ${BUILD_ROOT}/results.csv"
echo "Raw stats in:   ${BUILD_ROOT}/results"
