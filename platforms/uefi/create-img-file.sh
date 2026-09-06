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

if [ "$#" -eq 2 ]; then
    FAT_FOLDER_LOC="$(realpath "$1")"
    IMG_FILE="$(realpath "$2")"
else
    echo "Too many arguments given, expected 2" >&2
    exit 1
fi

set -x

SIZE_MB=100

validate_parent_dir "$IMG_FILE"

rm -f "$IMG_FILE"

# Create empty image
dd "if=/dev/zero" "of=$IMG_FILE" "bs=1M" "count=$SIZE_MB" "status=progress"

# 2048 sectors = 1 MiB offset
START_SECTOR_AMOUNT="2048"
SECTOR_SIZE="512"
PART_START="$((START_SECTOR_AMOUNT * 1))"
PART_END="$(((SIZE_MB * START_SECTOR_AMOUNT) - 1))"

# Create MBR partition table and one ESP (EF) partition, with the boot flag enabled
# Partition starts at 1 MiB for proper alignment.
fdisk "$IMG_FILE" <<EOF
o
n
p
1
$PART_START
$PART_END
t
0c
a
w
EOF

# make fat32 partition
mkfs.fat -a -S "$SECTOR_SIZE" -F 32 --offset="$PART_START" "$IMG_FILE"

PART_START_BYTES="$((START_SECTOR_AMOUNT * SECTOR_SIZE))"

mcopy -i "${IMG_FILE}@@$PART_START_BYTES" -s "$FAT_FOLDER_LOC/" "::/"

mdir -i "${IMG_FILE}@@$PART_START_BYTES" "::"

fdisk -l "$IMG_FILE"
