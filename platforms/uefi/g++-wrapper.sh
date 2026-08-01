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

COMPILED=()

# Capture all args
ARGS=("$@")

OUTPUT_FILE=""
NEXT_TYPE="unknown"

for ARG in "${ARGS[@]}"; do
    case "$ARG" in
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
            OUTPUT_FILE="$(normalize_file "$ARG")"
            NEXT_TYPE="unknown"
        elif [[ "$NEXT_TYPE" == "ignore" ]]; then
            NEXT_TYPE="unknown"
        else
            case "$ARG" in
            *.a | *.so)
                change_mode "link"
                DEPENDENCIES+=("$(resolve_file "$ARG")")
                ;;
            *.o)
                DEPENDENCIES+=("$(resolve_file "$ARG")")
                ;;
            *.cpp)
                COMPILED+=("$(resolve_file "$ARG")")
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
    "args": $(to_json_array "${ARGS[@]}"),
    "language": "cpp",
    "type": "compile",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "files": $(to_json_array "${DEPENDENCIES[@]}" "${COMPILED[@]}"),
        "dependencies": $(to_json_array "${DEPENDENCIES[@]}"),
        "compiled": $(to_json_array "${COMPILED[@]}")
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
    "args": $(to_json_array "${ARGS[@]}"),
    "language": "cpp",
    "type": "link",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "files": $(to_json_array "${DEPENDENCIES[@]}")
    }
}
EOF

else
    echo "<$TOOL> ${ARGS[*]}" >&2
    echo "Not recognized intent: $MODE" >&2
    exit 2
fi
