#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

BUILD_DIR="${PROJECT_DIR}/cmake-build-collector"

cmake \
    -S "${PROJECT_DIR}" \
    -B "${BUILD_DIR}" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++ \
    -DBUILD_TESTING=OFF \
    -DJOURNAL_ENABLE_SOCKET=ON

cmake --build "${BUILD_DIR}" \
    --target journal_collector \
    --parallel

exec "${BUILD_DIR}/collector/JournalCollector" "$@"