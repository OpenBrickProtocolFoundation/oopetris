#!/usr/bin/env bash

DEPENDENCIES=()

validate_dependencies() {

    for DEPENDENCY in "${DEPENDENCIES[@]}"; do
        FILE="$(realpath "$DEPENDENCY")"

        if ! (jq -e "." "$FILE" >/dev/null); then
            echo "<$TOOL> ${ARGS[*]}" >&2
            echo "Dependency '$FILE' is not a valid json file" >&2
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
