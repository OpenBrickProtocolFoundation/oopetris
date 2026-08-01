#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status.
set -e
## Treat undefined variables as an error
set -u
# fails if any part of a pipeline (|) fails
set -o pipefail

SCRIPT_DIR="$(realpath "$(dirname -- "${BASH_SOURCE[0]}")")"

# shellcheck source=./platforms/helper.sh
source "$SCRIPT_DIR/../helper.sh"

if [ "$#" -eq 2 ]; then
    BUILD_INF="$(realpath "$1")"
    DEST_FILE="$(realpath "$2")"
else
    echo "Too many arguments given, expected 2" >&2
    exit 1
fi

UEFI_INFO_FILE="$(realpath "$SCRIPT_DIR/../crossbuild/uefi_info.json")"
EDK2_BUILD_COMMAND="$(jq -M -r -c '.["build"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_ARCH="$(jq -M -r -c '.["arch"]' "$UEFI_INFO_FILE")"
WORKSPACE="$(jq -M -r -c '.["workspace"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="$(jq -M -r -c '.["platform"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_BUILDTYPE="$(jq -M -r -c '.["buildtype"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_TOOLCHAIN="$(jq -M -r -c '.["toolchain"]' "$UEFI_INFO_FILE")"

BUILD_NAME=$(basename -- $"$BUILD_INF")

BUILD_FILE_TARGET="GeneratedPackages/$BUILD_NAME"

BUILD_FILE_TARGET_ABS="$WORKSPACE/$BUILD_FILE_TARGET"

if ! [ -e "$BUILD_FILE_TARGET_ABS" ]; then
    echo "Build file is not in the workspace: $BUILD_FILE_TARGET" >&2
    exit 2
elif ! readlink "$BUILD_FILE_TARGET_ABS"; then
    echo "Build file is not a symlink: $BUILD_FILE_TARGET_ABS" >&2
    exit 2
else
    BUILD_TARGET_LINK_TARGET="$(readlink "$BUILD_FILE_TARGET_ABS")"

    if [ "$BUILD_TARGET_LINK_TARGET" != "$BUILD_INF" ]; then
        echo "Build file isn't linked correctly: '$BUILD_TARGET_LINK_TARGET' != '$BUILD_INF'" >&2
        exit 2
    fi
fi

"$EDK2_BUILD_COMMAND" -a "$EDK2_TARGET_PROPERTIES_ARCH" \
    -p "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" \
    -m "$BUILD_FILE_TARGET" \
    -b "$EDK2_TARGET_PROPERTIES_BUILDTYPE" \
    -t "$EDK2_TARGET_PROPERTIES_TOOLCHAIN" \
    -w # -v # verbose

OUTPUT_FILE="TODO"

# link the output .efi to the correct location

echo "TODO"
exit 45
