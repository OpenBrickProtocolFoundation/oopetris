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

TOOL="C-EMBED"
# Capture all args
ARGS=("$@")

# shellcheck source=./platforms/uefi/base.sh
source "$SCRIPT_DIR/base.sh"

if [ "$#" -eq 3 ]; then
    C_EMBED_EXE="$(realpath "$1")"
    INPUT_FOLDER="$(realpath "$2")"
    OUTPUT_FILE="$(realpath "$3")"
else
    echo "Too many arguments given, expected 3" >&2
    exit 1
fi

PARENT_DIR="$(dirname "$OUTPUT_FILE")"

if [[ ! -d "$PARENT_DIR" ]]; then
    mkdir -p "$PARENT_DIR"
fi

REAL_O_FILE="$PARENT_DIR/c-embed_compiled.o"

"$C_EMBED_EXE" -r -a elf64-x86-64 -o "$REAL_O_FILE" "$INPUT_FOLDER"

mv "./c-embed.o" "$REAL_O_FILE"

# create a "extern_object" json description for the automatic inf creation process to generate the inf for this file, as if it was compiled via the edk2 toolchain, but it wasn't
cat <<EOF >"$OUTPUT_FILE"
{
    "cwd": "$(pwd)",
    "args": $(to_json_array),
    "language": "c",
    "type": "extern_object",
    "dependencies": {
        "output": "$OUTPUT_FILE",
        "extern": $(to_json_array "$REAL_O_FILE")
    }
}
EOF
