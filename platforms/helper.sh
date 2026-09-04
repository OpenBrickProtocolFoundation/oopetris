#!/usr/bin/env bash

validate_parent_dir() {
    local FILE="$1"
    local PARENT_DIR
    PARENT_DIR="$(dirname "$FILE")"

    if [[ ! -d "$PARENT_DIR" ]]; then
        mkdir -p "$PARENT_DIR"
    fi
}

validate_abs() {
    local FILE="$1"

    if [[ "${FILE:0:1}" != "/" ]]; then
        echo "Not an absolute path: $FILE" >&2
        exit 7
    fi

}

link_files_checked() {
    local SOURCE="$1"
    local DEST="$2"

    validate_abs "$SOURCE"
    validate_abs "$DEST"

    ln -s "$SOURCE" "$DEST"

}
