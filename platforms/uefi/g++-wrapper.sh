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

# Capture all args
ARGS=("$@")
NEW_ARGS=()

MODE="unknown"
OUTPUT_FILE=""
NEXT_IS_OUTPUT="false"

for arg in "${ARGS[@]}"; do
    case "$arg" in
    -Wl,--version | -Wl,-v)
        MODE="pass"
        NEW_ARGS+=("$arg")
        ;;
    -c)
        MODE="compile"
        NEW_ARGS+=("$arg")
        ;;
    -o)
        NEW_ARGS+=("$arg")
        NEXT_IS_OUTPUT="true"
        ;;
    *.o)
        MODE="O_FILES"
        NEW_ARGS+=("$arg")
        ;;
    --version | -dM|-xc\+\+)
        MODE="pass"
        NEW_ARGS+=("$arg")
        ;;
    *)
        if [[ "$NEXT_IS_OUTPUT" == true ]]; then
            OUTPUT_FILE="$arg"
            NEXT_IS_OUTPUT=false
        fi
        NEW_ARGS+=("$arg")
        ;;
    esac
done

if [[ "$MODE" == "pass" ]]; then
    exec "$CXX" "${NEW_ARGS[@]}"
elif [[ "$MODE" == "compile" ]]; then
    if [ -z "$OUTPUT_FILE" ]; then
        echo "<G++> ${NEW_ARGS[*]}" >&2
        echo "Missing output file" >&2
        exit 2
    fi

    validate_parent_dir "$OUTPUT_FILE"

    cat <<EOF >"$OUTPUT_FILE"
{ 
    "invocation": $(printf '%s\n' "${ARGS[@]}" | jq -R . | jq -sc .),
    "args": $(printf '%s\n' "${NEW_ARGS[@]}" | jq -R . | jq -sc .),
    "type": "cpp"
}
EOF

else
    echo "<G++> ${NEW_ARGS[*]}" >&2
    echo "Not recognized intent: $MODE" >&2
    exit 2
fi
