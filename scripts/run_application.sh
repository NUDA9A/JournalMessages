#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

JOURNAL_ENABLE_SOCKET="${JOURNAL_ENABLE_SOCKET:-OFF}"
BUILD_DIR="${PROJECT_DIR}/cmake-build-application-${JOURNAL_ENABLE_SOCKET}"

cmake \
    -S "${PROJECT_DIR}" \
    -B "${BUILD_DIR}" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++ \
    -DBUILD_TESTING=OFF \
    -DJOURNAL_ENABLE_SOCKET="${JOURNAL_ENABLE_SOCKET}"

cmake --build "${BUILD_DIR}" \
    --target journal_application \
    --parallel

exec "${BUILD_DIR}/application/JournalMessages" "$@"