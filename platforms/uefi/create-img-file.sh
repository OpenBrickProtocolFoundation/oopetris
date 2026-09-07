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
    IMG_DISK_FILE="$(realpath "$2")"
    IMG_PART_FILE="$(realpath "$3")"
else
    echo "Too many arguments given, expected 3" >&2
    exit 1
fi

SIZE_MB=100
SECTOR_SIZE="512"

validate_parent_dir "$IMG_DISK_FILE"
validate_parent_dir "$IMG_PART_FILE"

rm -f "$IMG_DISK_FILE"
rm -f "$IMG_PART_FILE"

## 512b sectors
TOTAL_SECTORS_COUNT="$((SIZE_MB * 1024 * 1024 / SECTOR_SIZE))"

# Create empty image
dd "if=/dev/zero" "of=$IMG_DISK_FILE" "bs=$SECTOR_SIZE" "count=$TOTAL_SECTORS_COUNT" "status=progress"

# 1 MiB offset
START_SECTOR_AMOUNT="$((1024 * 1024 / SECTOR_SIZE))"
PART_START="$((START_SECTOR_AMOUNT * 1))"
PART_END="$(((SIZE_MB * START_SECTOR_AMOUNT) - 1))"

# Create MBR partition table and one ESP (EF) partition, with the boot flag enabled
# Partition starts at 1 MiB for proper alignment.
fdisk "$IMG_DISK_FILE" <<EOF
o
n
p
1
$PART_START
$PART_END
t
uefi
a
w
EOF

## 512b sectors
FAT_IMG_SECTORS_COUNT="$((PART_END - PART_START + 1))"

## create temporary image, where we create raw fat32 partition
dd "if=/dev/zero" "of=$IMG_PART_FILE" "bs=$SECTOR_SIZE" "count=$FAT_IMG_SECTORS_COUNT" "status=progress"

# make fat32 partition
mkfs.fat -F 32 -n "OOPetrisEfi" "$IMG_PART_FILE"

while IFS= read -r -d '' FILE; do
    mcopy -i "$IMG_PART_FILE" -s "$FILE" "::/"
done < <(find "$FAT_FOLDER_LOC" -mindepth 1 -maxdepth 1 -print0)

mdir -i "$IMG_PART_FILE" "::"

## copy the raw fat32 partition over to the whole img file
dd "if=$IMG_PART_FILE" "of=$IMG_DISK_FILE" "bs=$SECTOR_SIZE" "count=$FAT_IMG_SECTORS_COUNT" "seek=$PART_START" "conv=notrunc" "status=progress"

fsck.fat -vn "$IMG_PART_FILE"

fdisk -l "$IMG_DISK_FILE"
