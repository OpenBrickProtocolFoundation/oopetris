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

TOOL="AR"
AR="gcc-ar-15"

# shellcheck source=./platforms/uefi/base.sh
source "$SCRIPT_DIR/base.sh"

# Capture all args
ARGS=("$@")

OUTPUT_FILE=""
NEXT_TYPE="unknown"

for arg in "${ARGS[@]}"; do
    case "$arg" in
    --version)
        change_mode "pass"
        ;;
    csr)
        change_mode "create"
        NEXT_TYPE="output"
        ;;
    *)
        if [[ "$NEXT_TYPE" == "output" ]]; then
            OUTPUT_FILE="$arg"
            NEXT_TYPE="unknown"
        elif [[ "$NEXT_TYPE" == "ignore" ]]; then
            NEXT_TYPE="unknown"
        else
            case "$arg" in
            *.a | *.so | *.o)
                DEPENDENCIES+=("$arg")
                ;;
            *) ;;
            esac
        fi
        ;;
    esac
done

if [[ "$MODE" == "pass" ]]; then
    exec "$AR" "${ARGS[@]}"
elif [[ "$MODE" == "create" ]]; then
    if [ -z "$OUTPUT_FILE" ]; then
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Missing output file" >&2
        exit 2
    fi

    validate_parent_dir "$OUTPUT_FILE"

    validate_dependencies

    cat <<EOF >"$OUTPUT_FILE"
{
    "cwd": "$(pwd)",
    "args": $(to_json_array "${ARGS[@]}"),
    "language": "ar",
    "type": "archive",
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
