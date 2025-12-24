#!/usr/bin/env bash
#
# NeoC build script - Cross-platform build helper
#

set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="${1:-release}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

info() { echo -e "${GREEN}[INFO]${NC} $*"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }

usage() {
    cat << EOF
Usage: $0 [command]

Commands:
    debug       Build debug configuration with sanitizers
    release     Build optimized release configuration (default)
    clean       Remove build artifacts
    test        Build and run tests
    install     Install to system (requires sudo on Unix)
    help        Show this help message

Examples:
    $0              # Build release
    $0 debug        # Build debug with sanitizers
    $0 clean        # Clean build directory
EOF
}

check_deps() {
    local missing=()

    if ! command -v cmake &> /dev/null; then
        missing+=("cmake")
    fi

    if ! command -v ninja &> /dev/null && ! command -v make &> /dev/null; then
        missing+=("ninja or make")
    fi

    if (( ${#missing[@]} > 0 )); then
        error "Missing dependencies: ${missing[*]}"
        exit 1
    fi
}

detect_platform() {
    case "$(uname -s)" in
        Darwin*)  echo "macos" ;;
        Linux*)   echo "linux" ;;
        MINGW*|MSYS*|CYGWIN*) echo "windows" ;;
        *)        echo "unknown" ;;
    esac
}

build() {
    local build_type="$1"
    local build_dir="${PROJECT_DIR}/build/${build_type}"
    local generator=""

    # Prefer Ninja if available
    if command -v ninja &> /dev/null; then
        generator="-G Ninja"
    fi

    info "Building ${build_type} configuration..."

    mkdir -p "$build_dir"

    local cmake_args=(
        -S "$PROJECT_DIR"
        -B "$build_dir"
        $generator
    )

    case "$build_type" in
        debug)
            cmake_args+=(
                -DCMAKE_BUILD_TYPE=Debug
                -DNEOC_ENABLE_SANITIZERS=ON
                -DNEOC_ENABLE_LTO=OFF
            )
            ;;
        release)
            cmake_args+=(
                -DCMAKE_BUILD_TYPE=Release
                -DNEOC_ENABLE_SANITIZERS=OFF
                -DNEOC_ENABLE_LTO=ON
            )
            ;;
        *)
            error "Unknown build type: $build_type"
            exit 1
            ;;
    esac

    cmake "${cmake_args[@]}"
    cmake --build "$build_dir" --parallel "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

    # Update symlink for IDE support
    ln -sf "${build_dir}/compile_commands.json" "${PROJECT_DIR}/compile_commands.json"

    info "Build complete: ${build_dir}/neoc"
}

clean() {
    info "Cleaning build artifacts..."
    rm -rf "${PROJECT_DIR}/build"
    rm -rf "${PROJECT_DIR}/bin"
    rm -rf "${PROJECT_DIR}/obj"
    info "Clean complete"
}

run_tests() {
    local build_dir="${PROJECT_DIR}/build/debug"

    info "Building and running tests..."
    cmake -S "$PROJECT_DIR" -B "$build_dir" -DNEOC_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
    cmake --build "$build_dir" --parallel
    ctest --test-dir "$build_dir" --output-on-failure
}

install_build() {
    local build_dir="${PROJECT_DIR}/build/release"

    if [[ ! -f "${build_dir}/neoc" ]]; then
        build release
    fi

    info "Installing..."
    cmake --install "$build_dir"
}

main() {
    check_deps

    case "${BUILD_TYPE}" in
        debug|release)
            build "$BUILD_TYPE"
            ;;
        clean)
            clean
            ;;
        test)
            run_tests
            ;;
        install)
            install_build
            ;;
        help|-h|--help)
            usage
            ;;
        *)
            error "Unknown command: $BUILD_TYPE"
            usage
            exit 1
            ;;
    esac
}

main "$@"
