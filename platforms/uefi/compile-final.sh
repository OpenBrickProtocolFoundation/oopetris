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

"$EDK2_BUILD_COMMAND" -a "$EDK2_TARGET_PROPERTIES_ARCH" \
    -p "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" \
    -m "$BUILD_INF" \
    -b "$EDK2_TARGET_PROPERTIES_BUILDTYPE" \
    -t "$EDK2_TARGET_PROPERTIES_TOOLCHAIN" \
    -w # -v # verbose

OUTPUT_FILE="TODO"

# link the output .efi to the correct location

echo "TODO"
exit 45
