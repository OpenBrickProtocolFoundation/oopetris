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

rm -f "$IMG_DISK_FILE"

## 512b sectors
TOTAL_SECTORS_COUNT="$((SIZE_MB * 1024 * 1024 / SECTOR_SIZE))"

# Create empty image
dd "if=/dev/zero" "of=$IMG_DISK_FILE" "bs=$SECTOR_SIZE" "count=$TOTAL_SECTORS_COUNT" "status=progress"

# Create MBR partition table and one ESP (EF) partition, with the boot flag enabled
# Partition starts at 1 MiB for proper alignment.
fdisk "$IMG_DISK_FILE" <<EOF
o
n
p
1


t
uefi
a
w
EOF

LOOP_OUTPUT="$(udisksctl loop-setup --no-user-interaction --file "$IMG_DISK_FILE")"

echo "$LOOP_OUTPUT"

LOOP_FILE=$(printf '%s\n' "$LOOP_OUTPUT" | sed -n 's/.* as \(\/dev\/loop[0-9]\+\)\..*/\1/p')

cleanup() {
    udisksctl loop-delete --no-user-interaction -b "$LOOP_FILE"
}
trap cleanup EXIT

lsblk "${LOOP_FILE}"

LOOP_PART="${LOOP_FILE}p1"

# make fat32 partition
mkfs.fat -F 32 -n "OOPetrisEfi" "$LOOP_PART"

MOUNT_OUTPUT="$(udisksctl mount --no-user-interaction -b "$LOOP_PART")"

echo "$MOUNT_OUTPUT"

MOUNT_PATH="${MOUNT_OUTPUT##* at }"

while IFS= read -r -d '' FILE; do
    cp -r "$FILE" "$MOUNT_PATH/"
done < <(find "$FAT_FOLDER_LOC" -mindepth 1 -maxdepth 1 -print0)

udisksctl unmount --no-user-interaction -b "$LOOP_PART"

fdisk -l "$IMG_DISK_FILE"
