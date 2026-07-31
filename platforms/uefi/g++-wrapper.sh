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

CXX="g++-15"
TOOL="G++"

# Capture all args
ARGS=("$@")

DEPENDENCIES=()
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

OUTPUT_FILE=""
NEXT_IS_OUTPUT="false"

for arg in "${ARGS[@]}"; do
    case "$arg" in
    -Wl,--version | -Wl,-v)
        change_mode "pass"
        ;;
    -c)
        change_mode "compile"
        ;;
    -Wl,*)
        change_mode "link"
        ;;
    -o)
        NEXT_IS_OUTPUT="true"
        ;;
    --version | -dM | -xc\+\+)
        change_mode "pass"
        ;;
    *)
        if [[ "$NEXT_IS_OUTPUT" == true ]]; then
            OUTPUT_FILE="$arg"
            NEXT_IS_OUTPUT=false
        else
            case "$arg" in
            *.a | *.so)
                change_mode "link"
                DEPENDENCIES+=("$arg")
                ;;
            *.o)
                DEPENDENCIES+=("$arg")
                ;;
            *) ;;
            esac
        fi
        ;;
    esac
done

if [[ "$MODE" == "pass" ]]; then
    exec "$CXX" "${ARGS[@]}"
elif [[ "$MODE" == "compile" ]]; then
    if [ -z "$OUTPUT_FILE" ]; then
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Missing output file" >&2
        exit 2
    fi

    validate_parent_dir "$OUTPUT_FILE"

    cat <<EOF >"$OUTPUT_FILE"
{ 
    "cwd": "$(pwd)",
    "args": $(printf '%s\n' "${ARGS[@]}" | jq -R . | jq -sc .),
    "language": "cpp",
    "type": "compile",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "files": $(printf '%s\n' "${DEPENDENCIES[@]}" | jq -R . | jq -sc .)
    }
}
EOF

elif [[ "$MODE" == "link" ]]; then
    if [ -z "$OUTPUT_FILE" ]; then
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Missing output file" >&2
        exit 2
    fi

    validate_parent_dir "$OUTPUT_FILE"

    cat <<EOF >"$OUTPUT_FILE"
{ 
    "cwd": "$(pwd)",
    "args": $(printf '%s\n' "${ARGS[@]}" | jq -R . | jq -sc .),
    "language": "cpp",
    "type": "link",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "files": $(printf '%s\n' "${DEPENDENCIES[@]}" | jq -R . | jq -sc .)
    }
}
EOF

else
    echo "<$TOOL> ${ARGS[*]}" >&2
    echo "Not recognized intent: $MODE" >&2
    exit 2
fi
