#!/usr/bin/env bash

function process_icon() {
  SRC=$1
  RES=$(echo "$SRC" | awk -F_ '{ print $2 }' | awk -F. '{ print $1 }' | awk '{ print $1 }')

  DEST_DIR="/app/share/icons/hicolor/${RES}x${RES}/apps"
  mkdir -p "$DEST_DIR"
  cp "$SRC" "$DEST_DIR/com.github.mgerhold.OOPetris.png"
}

export -f process_icon

#TODO get real logo and use more sizes!
find ../assets/icon/ -name "test_*.png" -exec bash -c 'process_icon "$@";' -- {} \;

## TODO svg /app/share/icons/hicolor/scalable/apps/com.github.mgerhold.OOPetris.svg
