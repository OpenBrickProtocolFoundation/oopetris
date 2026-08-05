#!/usr/bin/env bash

validate_dependencies() {
    local DEPENDENCIES=("$@")

    for DEPENDENCY in "${DEPENDENCIES[@]}"; do
        FILE="$(realpath "$DEPENDENCY")"

        if ! (jq -e "." "$FILE" >/dev/null); then
            echo "<$TOOL> ${ARGS[*]}" >&2
            echo "Dependency '$FILE' is not a valid json file" >&2
            exit 5
        fi

    done
}

validate_files() {
    local FILES=("$@")

    for FILE in "${FILES[@]}"; do
        FILE="$(realpath "$FILE")"

        if ! [ -e "$FILE" ]; then
            echo "<$TOOL> ${ARGS[*]}" >&2
            echo "File '$FILE' does not exist" >&2
            exit 5
        fi

    done
}

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

NEXT_TYPE="unknown"

change_next_type() {
    local NEW_TYPE="$1"

    if [[ "$NEXT_TYPE" == "unknown" ]]; then
        NEXT_TYPE="$NEW_TYPE"
    elif [[ "$NEXT_TYPE" == "$NEW_TYPE" ]]; then
        :
    else
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Can't change next type from $NEXT_TYPE to $NEW_TYPE" >&2
        exit 4
    fi
}

reset_next_type() {
    NEXT_TYPE="unknown"
}

require_empty_array() {
    local ARRAY=("$@")

    if [ "${#ARRAY[@]}" -ne 0 ]; then
        echo "Array is not empty but ${#ARRAY[@]} long" >&2
        exit 9
    fi
}

to_json_array() {
    local ARRAY=("$@")

    if [ "${#ARRAY[@]}" -eq 0 ]; then
        echo "[]"
    else
        printf '%s\n' "${ARRAY[@]}" | jq -R . | jq -sc .
    fi
}

resolve_file() {
    local FILE="$1"

    if ! [ -e "$FILE" ]; then
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Can't resolve file '$FILE': not found" >&2
        exit 6
    fi

    realpath "$FILE"

}

normalize_file() {
    local FILE="$1"

    realpath "$FILE"
}

is_soname() {
    local SO_NAME="$1"
    [[ "$SO_NAME" =~ (^|/)lib[^/]+\.so(\.[0-9]+)*$ ]]
}
