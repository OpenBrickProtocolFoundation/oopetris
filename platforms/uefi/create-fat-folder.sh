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

# create a grub config to boot this

GRUB_BOOT_FOLDER_LOC="$FAT_FOLDER_LOC/boot/grub/"

mkdir -p "$GRUB_BOOT_FOLDER_LOC"

OOPETRIS_PART_FILE="$GRUB_BOOT_FOLDER_LOC/oopetris_marker.txt"

echo "OOPETRIS MARKER" >"$OOPETRIS_PART_FILE"

GRUB_CFG_FILE="$GRUB_BOOT_FOLDER_LOC/grub.cfg"

cat <<EOF >"$GRUB_CFG_FILE"
menuentry "Boot OOPetris EFI" --class efi {
    ## not sure which of these are really needed, but load them, so we don#t rely on defaults
    insmod boot
    insmod chain
    insmod fat
    insmod msdospart
    insmod search
    insmod search_fs_file
    # search for the marker file and then set the root to that partition
    search --no-floppy --file --set=root /boot/grub/oopetris_marker.txt
    # set this file as boot entry
    chainloader /EFI/BOOT/BOOTX64.EFI
    # boot the file
    boot
}
EOF

#NOTE: this makes booting easier, just run
#'configfile /boot/grub/grub.cfg' in the grub shell
# or maybe grub detects this config file and show it in the graphical interface (?)
# TODO: when does grub do this?
# NOTE: select the disk in the grub shell by using ls and then "(hd0,msdos)/.."

touch "$PSEUDO_OUTPUT"
