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

LIBRARIES_FILE="$(realpath "$SCRIPT_DIR/../../src/executables/platforms/uefi/libraries.json")"

PKG_CONFIG="pkg-config"
TOOL="PKG-CONFIG"

# Capture all ARGs
ARGS=("$@")

MODE="unknown"

change_mode() {
    local NEW_MODE="$1"

    if [[ "$MODE" == "unknown" ]]; then
        MODE="$NEW_MODE"
    elif [[ "$MODE" == "$NEW_MODE" ]]; then
        :
    else
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Can't change mode from $MODE to $NEW_MODE" >&2
        exit 4
    fi
}

WHAT=""
PACKAGE_NAME=""
NEXT_IS_PACKAGE_NAME="false"

for ARG in "${ARGS[@]}"; do
    case "$ARG" in
    --version)
        change_mode "pass"
        ;;
    --modversion)
        change_mode "get"
        WHAT="version"
        NEXT_IS_PACKAGE_NAME="true"
        ;;
    --cflags)
        change_mode "get"
        WHAT="cflags"
        NEXT_IS_PACKAGE_NAME="true"
        ;;
    --libs)
        change_mode "get"
        WHAT="libflags"
        NEXT_IS_PACKAGE_NAME="true"
        ;;
    *)
        if [[ "$NEXT_IS_PACKAGE_NAME" == true ]]; then
            PACKAGE_NAME="$ARG"
            NEXT_IS_PACKAGE_NAME=false
        fi
        ;;
    esac
done

if [[ "$MODE" == "pass" ]]; then
    exec "$PKG_CONFIG" "${ARGS[@]}"
elif [[ "$MODE" == "get" ]]; then
    if [ -z "$PACKAGE_NAME" ]; then
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Missing package name" >&2
        exit 2
    fi

    LIBRARY_ENTRY="$(jq -M -r -c ".[\"${PACKAGE_NAME}\"]" "$LIBRARIES_FILE")"

    if [ "$LIBRARY_ENTRY" = "null" ]; then
        exit 1
    fi

    if [[ "$WHAT" == "version" ]]; then
        echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"version\"]"
        exit 0
    elif [[ "$WHAT" == "cflags" ]]; then
        echo "-t:use-lib:name=$PACKAGE_NAME"
        echo "-t:use-lib:pkg=$(echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"pkg\"]")"
        exit 0
    elif [[ "$WHAT" == "libflags" ]]; then
        echo "-Wl,-t:use-lib:name=$PACKAGE_NAME"
        echo "-Wl,-t:use-lib:pkg=$(echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"pkg\"]")"
        exit 0
    else
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Not recognized intent: $MODE" >&2
        exit 3
    fi

else
    echo "<$TOOL> ${ARGS[*]}" >&2
    echo "Not recognized intent: $MODE" >&2
    exit 2
fi
