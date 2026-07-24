#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

JOURNAL_ENABLE_SOCKET="${JOURNAL_ENABLE_SOCKET:-ON}"
BUILD_DIR="${PROJECT_DIR}/cmake-build-tests-${JOURNAL_ENABLE_SOCKET}"

cmake \
    -S "${PROJECT_DIR}" \
    -B "${BUILD_DIR}" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=g++ \
    -DBUILD_TESTING=ON \
    -DJOURNAL_ENABLE_SOCKET="${JOURNAL_ENABLE_SOCKET}"

cmake --build "${BUILD_DIR}" \
    --target journal_tests journal_tests_shared \
    --parallel

ctest \
    --test-dir "${BUILD_DIR}" \
    --output-on-failure