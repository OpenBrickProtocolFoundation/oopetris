#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status.
set -e
## Treat undefined variables as an error
set -u
# fails if any part of a pipeline (|) fails
set -o pipefail

if [ "$#" -eq 0 ]; then
    # nothing
    echo "At least one argument required"
fi

SCRIPT_DIR="$(realpath "$(dirname -- "${BASH_SOURCE[0]}")")"

PLATFORM="$1"
shift

SUPPORTED_PLATFORMS=("3ds" "android" "switch" "uefi" "web")

for PLTFRM in "${SUPPORTED_PLATFORMS[@]}"; do
    if [ "$PLATFORM" == "$PLTFRM" ]; then
        "$SCRIPT_DIR/build-$PLTFRM.sh" "$@"
        EXIT_CODE="$?"
        exit "$EXIT_CODE"
    fi

done

echo "Invalid PLATFORM, expected one of: ${SUPPORTED_PLATFORMS[*]}" >&2
exit 1
