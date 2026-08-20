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

UEFI_INFO_FILE="$(realpath "$SCRIPT_DIR/../crossbuild/uefi_info.json")"
EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="$(jq -M -r -c '.["platform"]' "$UEFI_INFO_FILE")"
EDK2_TOOLS_DIR="$(jq -M -r -c '.["edk2_tools_dir"]' "$UEFI_INFO_FILE")"

link_uefi_libray() {
    local LIB_NAME_INF="$1"
    local PACKAGE_NAME_INC="$2"

    if grep -q "$LIB_NAME_INF" "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"; then
        : # found already, do nothing
    else
        cat <<EOF >>"$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"
[Components]
  $LIB_NAME_INF

EOF

    fi

    if grep -q "$PACKAGE_NAME_INC" "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"; then
        : # found already, do nothing
    else
        cat <<EOF >>"$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"
!include $PACKAGE_NAME_INC

EOF

    fi

}

link_built_libray() {
    local PACKAGE_NAME_PKG="$1"

    local FINAL_PKG_NAME="$PACKAGE_NAME_PKG/$PACKAGE_NAME_PKG.inf"

    local FINAL_PKG_INC_NAME="$PACKAGE_NAME_PKG/$PACKAGE_NAME_PKG.inc"

    link_uefi_libray "$FINAL_PKG_NAME" "$FINAL_PKG_INC_NAME"

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

    case "$PACKAGE_NAME" in
    uefi:*)
        # special handling

        LIB_ENTRY_NAME_UEFI="${PACKAGE_NAME:5}"

        LIBRARY_ENTRY_UEFI="$(jq -M -r -c ".[\"uefi\"][\"${LIB_ENTRY_NAME_UEFI}\"]" "$LIBRARIES_FILE")"

        if [ "$LIBRARY_ENTRY_UEFI" = "null" ]; then
            exit 1
        fi

        PACKAGE_NAME_UEFI="$(echo "$LIBRARY_ENTRY_UEFI" | jq -M -r -c ".[\"pkg\"]")"
        LIB_NAME_UEFI="$(echo "$LIBRARY_ENTRY_UEFI" | jq -M -r -c ".[\"lib\"]")"
        INF_FILE_UEFI="$(echo "$LIBRARY_ENTRY_UEFI" | jq -M -r -c ".[\"inf\"]")"
        INC_FILE_UEFI="$(echo "$LIBRARY_ENTRY_UEFI" | jq -M -r -c ".[\"inc\"]")"
        TOOL_PREFIX_UEFI="$(echo "$LIBRARY_ENTRY_UEFI" | jq -M -r -c ".[\"tool_prefix\"]")"

        PACKAGE_NAME_DEC="$PACKAGE_NAME_UEFI.dec"
        PACKAGE_DESC_FILE="$EDK2_TOOLS_DIR/$TOOL_PREFIX_UEFI/$PACKAGE_NAME_DEC"

        if ! [ -e "$PACKAGE_DESC_FILE" ]; then
            exit 1
        fi

        if [[ "$WHAT" == "version" ]]; then
            PACKAGE_VERSION=$(grep -E '^[[:space:]]*PACKAGE_VERSION[[:space:]]*=' "$PACKAGE_DESC_FILE" | cut -d= -f2 | xargs)
            echo "$PACKAGE_VERSION"
            exit 0
        elif [[ "$WHAT" == "cflags" ]]; then
            echo "-t:use-pkg:name=$PACKAGE_NAME_UEFI"
            echo "-t:use-pkg:pkg=$PACKAGE_NAME_DEC"
            echo "-t:use-pkg:lib=$LIB_NAME_UEFI"

            link_uefi_libray "$INF_FILE_UEFI" "$INC_FILE_UEFI"

            exit 0
        elif [[ "$WHAT" == "libflags" ]]; then

            echo "-Wl,-t:use-pkg:name=$PACKAGE_NAME_UEFI"
            echo "-Wl,-t:use-pkg:pkg=$PACKAGE_NAME_DEC"
            echo "-Wl,-t:use-pkg:lib=$LIB_NAME_UEFI"

            link_uefi_libray "$INF_FILE_UEFI" "$INC_FILE_UEFI"

            exit 0
        else
            echo "<$TOOL> ${ARGS[*]}" >&2
            echo "Not recognized intent: $MODE" >&2
            exit 3
        fi
        ;;
    *)
        # fall through
        ;;
    esac

    LIBRARY_ENTRY="$(jq -M -r -c ".[\"custom\"][\"${PACKAGE_NAME}\"]" "$LIBRARIES_FILE")"

    if [ "$LIBRARY_ENTRY" = "null" ]; then
        exit 1
    fi

    if [[ "$WHAT" == "version" ]]; then
        echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"version\"]"
        exit 0
    elif [[ "$WHAT" == "cflags" ]]; then
        PACKAGE_NAME_PKG="$(echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"pkg\"]")"
        PACKAGE_NAME_LIB="$(echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"lib\"]")"

        echo "-t:use-lib:name=$PACKAGE_NAME"
        echo "-t:use-lib:pkg=$PACKAGE_NAME_PKG"
        echo "-t:use-lib:lib=$PACKAGE_NAME_LIB"

        link_built_libray "$PACKAGE_NAME_PKG"

        exit 0
    elif [[ "$WHAT" == "libflags" ]]; then
        PACKAGE_NAME_PKG="$(echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"pkg\"]")"
        PACKAGE_NAME_LIB="$(echo "$LIBRARY_ENTRY" | jq -M -r -c ".[\"lib\"]")"

        echo "-Wl,-t:use-lib:name=$PACKAGE_NAME"
        echo "-Wl,-t:use-lib:pkg=$PACKAGE_NAME_PKG"
        echo "-Wl,-t:use-lib:lib=$PACKAGE_NAME_LIB"

        link_built_libray "$PACKAGE_NAME_PKG"

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
