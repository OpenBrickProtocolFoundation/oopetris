#!/usr/bin/env bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

SRC_DIR=$(realpath "$SCRIPT_DIR/..")

for RES in 24 48 64 72 96 128 144 160 192 256 512 1024; do

    inkscape "--export-width=$RES" "--export-height=$RES" --export-type=png "--export-filename=$SRC_DIR/assets/icon/${RES}x${RES}.png" "$SRC_DIR/assets/OOPetris.svg"

done

#TODO: this doesn't work correctly!
MAX_RES="4096"
inkscape "--export-width=$MAX_RES" "--export-height=$MAX_RES" --export-type=svg "--export-filename=$SRC_DIR/assets/icon/scalable.svg" "$SRC_DIR/assets/OOPetris.svg"
