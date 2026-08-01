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

# shellcheck source=./platforms/uefi/base.sh
source "$SCRIPT_DIR/base.sh"

# Capture all args
ARGS=("$@")

OUTPUT_FILE=""
NEXT_TYPE="unknown"

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
        NEXT_TYPE="output"
        ;;
    -MQ)
        NEXT_TYPE="ignore"
        ;;
    --version | -dM | -xc\+\+)
        change_mode "pass"
        ;;
    *)
        if [[ "$NEXT_TYPE" == "output" ]]; then
            OUTPUT_FILE="$arg"
            NEXT_TYPE="unknown"
        elif [[ "$NEXT_TYPE" == "ignore" ]]; then
            NEXT_TYPE="unknown"
        else
            case "$arg" in
            *.a | *.so)
                change_mode "link"
                DEPENDENCIES+=("$arg")
                ;;
            *.o)
                DEPENDENCIES+=("$arg")
                ;;
            *.cpp)
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
