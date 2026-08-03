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

CC="gcc-15"
TOOL="GCC"

# shellcheck source=./platforms/uefi/base.sh
source "$SCRIPT_DIR/base.sh"

# Capture all args
ARGS=("$@")

OUTPUT_FILE=""
DEPENDENCIES_O=()
DEPENDENCIES_SRC=()
DEPENDENCIES_LINK=()

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
        LINK_ARG="${ARG:4}"
        case "$LINK_ARG" in
        --as-needed | --no-undefined | -O* | -soname,* | -rpath,*)
            # ignore, valid arguments
            ;;
        --start-group)
            change_next_type "link"
            ;;
        --end-group)
            reset_next_type
            ;;
        -t:use-lib*)
            # ignore here, the extraction uses that for inf and dec dependencies
            ;;
        *)
            echo "Invalid link argument detected: '$LINK_ARG'" >&2
            exit 6
            ;;
        esac
        ;;
    -o)
        change_next_type "output"
        ;;
    -MQ | -MF)
        change_next_type "ignore"
        ;;
    --version | -dM)
        change_mode "pass"
        ;;
    -x)
        change_mode "pass"
        change_next_type "ignore"
        ;;
    -xc)
        # C specific
        change_mode "pass"
        ;;
    *)
        if [[ "$NEXT_TYPE" == "output" ]]; then
            OUTPUT_FILE="$(normalize_file "$ARG")"
            reset_next_type
        elif [[ "$NEXT_TYPE" == "ignore" ]]; then
            reset_next_type
        elif [[ "$NEXT_TYPE" == "link" ]]; then
            case "$ARG" in
            -l*)
                DEPENDENCIES_LINK+=("SYSTEM:${ARG:2}")
                ;;
            *.o)
                echo "Invalid file detected: '$ARG'" >&2
                exit 5
                ;;
            *.c)
                # C specific
                echo "Invalid file detected: '$ARG'" >&2
                exit 5
                ;;
            *.a | *.so)
                DEPENDENCIES_LINK+=("$(resolve_file "$ARG")")
                ;;
            *)
                if is_soname "$ARG"; then
                    DEPENDENCIES_LINK+=("$(resolve_file "$ARG")")
                else
                    echo "Invalid link group argument detected: '$ARG'" >&2
                    exit 6
                fi
                ;;
            esac
        elif [[ "$NEXT_TYPE" == "unknown" ]]; then
            case "$ARG" in
            *.o)
                DEPENDENCIES_O+=("$(resolve_file "$ARG")")
                ;;
            *.c)
                # C specific
                DEPENDENCIES_SRC+=("$(resolve_file "$ARG")")
                ;;
            *.a | *.so)
                DEPENDENCIES_LINK+=("$(resolve_file "$ARG")")
                ;;
            -cpp)
                # ignore this for meson usage, even if gcc only uses this for fortran, but it doesn't complain about it
                ;;
            -v | -E | - | -g | -MD | -pthread | -shared | -std=* | -f* | -m* | -D* | -U* | -W* | -O* | -I*)
                # ignore, valid arguments
                ;;
            -t:use-lib*)
                # ignore here, the extraction uses that for inf and dec dependencies
                ;;
            *)
                if is_soname "$ARG"; then
                    DEPENDENCIES_LINK+=("$(resolve_file "$ARG")")
                else

                    echo "Invalid argument detected: '$ARG'" >&2
                    exit 6
                fi
                ;;
            esac
        else
            echo "Not recognized next type: $NEXT_TYPE" >&2
            exit 11
        fi
        ;;
    esac
done

if [[ "$MODE" == "pass" ]]; then
    exec "$CC" "${ARGS[@]}"
elif [[ "$MODE" == "compile" ]]; then
    if [ -z "$OUTPUT_FILE" ]; then
        echo "<$TOOL> ${ARGS[*]}" >&2
        echo "Missing output file" >&2
        exit 2
    fi

    validate_parent_dir "$OUTPUT_FILE"

    validate_files "${DEPENDENCIES_SRC[@]}"

    require_empty_array "${DEPENDENCIES_O[@]}"

    require_empty_array "${DEPENDENCIES_LINK[@]}"

    cat <<EOF >"$OUTPUT_FILE"
{
    "cwd": "$(pwd)",
    "args": $(to_json_array "${ARGS[@]}"),
    "language": "c",
    "type": "compile",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "src": $(to_json_array "${DEPENDENCIES_SRC[@]}")
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

    validate_dependencies "${DEPENDENCIES_O[@]}"

    validate_dependencies "${DEPENDENCIES_LINK[@]}"

    require_empty_array "${DEPENDENCIES_SRC[@]}"

    cat <<EOF >"$OUTPUT_FILE"
{
    "cwd": "$(pwd)",
    "args": $(to_json_array "${ARGS[@]}"),
    "language": "c",
    "type": "link",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "o_files": $(to_json_array "${DEPENDENCIES_O[@]}"),
        "link_files": $(to_json_array "${DEPENDENCIES_LINK[@]}")
    }
}
EOF

else
    echo "<$TOOL> ${ARGS[*]}" >&2
    echo "Not recognized intent: $MODE" >&2
    exit 2
fi
