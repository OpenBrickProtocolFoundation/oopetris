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

if [ "$#" -eq 3 ]; then
    FAT_FOLDER_LOC="$(realpath "$1")"
    EFI_FILE="$(realpath "$2")"
    PSEUDO_OUTPUT="$(realpath "$3")"
else
    echo "Too many arguments given, expected 3" >&2
    exit 1
fi

rm -rf "$FAT_FOLDER_LOC"

BOOT_FOLDER_LOC="$FAT_FOLDER_LOC/EFI/BOOT"

mkdir -p "$BOOT_FOLDER_LOC"

DEST_FILE="$BOOT_FOLDER_LOC/BOOTX64.EFI"

# copy the output .efi to the correct location
if ! [ -e "$DEST_FILE" ]; then
    cp -p -f "$EFI_FILE" "$DEST_FILE"
fi

touch "$PSEUDO_OUTPUT"
