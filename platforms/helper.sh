#!/usr/bin/env bash

validate_parent_dir() {
    local FILE="$1"
    local PARENT_DIR
    PARENT_DIR="$(dirname "$FILE")"

    if [[ ! -d "$PARENT_DIR" ]]; then
        mkdir -p "$PARENT_DIR"
    fi
}
