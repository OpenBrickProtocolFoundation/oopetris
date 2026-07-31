#!/usr/bin/env bash

set -eu

# Capture all args
ARGS=("$@")
NEW_ARGS=()

for ARG in "${ARGS[@]}"; do
    echo "$ARG"
done

exit 2
