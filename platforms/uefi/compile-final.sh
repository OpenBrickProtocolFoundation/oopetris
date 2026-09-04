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
    BUILD_INF="$(realpath "$1")"
    DEST_FILE="$(realpath "$2")"
    DEST_FILE_ORIG="$2"
    DEPS_DEST_FILE="$(realpath "$3")"
else
    echo "Too many arguments given, expected 2" >&2
    exit 1
fi

UEFI_INFO_FILE="$(realpath "$SCRIPT_DIR/../crossbuild/uefi_info.json")"
EDK2_BUILD_COMMAND="$(jq -M -r -c '.["build"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_ARCH="$(jq -M -r -c '.["arch"]' "$UEFI_INFO_FILE")"
WORKSPACE="$(jq -M -r -c '.["workspace"]' "$UEFI_INFO_FILE")"
EDK2_TOOLS_DIR="$(jq -M -r -c '.["edk2_tools_dir"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="$(jq -M -r -c '.["platform"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME="$(jq -M -r -c '.["platform_name"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_BUILDTYPE="$(jq -M -r -c '.["buildtype"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_TOOLCHAIN="$(jq -M -r -c '.["toolchain"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_RUNTIME_TARGET="$(jq -M -r -c '.["runtime_target"]' "$UEFI_INFO_FILE")"

BUILD_NAME=$(basename -- $"$BUILD_INF")

BUILD_FILE_TARGET="GeneratedPackages/$BUILD_NAME"

BUILD_FILE_TARGET_ABS="$EDK2_TOOLS_DIR/$BUILD_FILE_TARGET"

if ! [ -e "$BUILD_FILE_TARGET_ABS" ]; then
    echo "Build file is not in the expected place: $BUILD_FILE_TARGET" >&2
    exit 2
elif ! readlink "$BUILD_FILE_TARGET_ABS"; then
    echo "Build file is not a symlink: $BUILD_FILE_TARGET_ABS" >&2
    exit 2
else
    BUILD_TARGET_LINK_TARGET="$(readlink "$BUILD_FILE_TARGET_ABS")"

    if [ "$BUILD_TARGET_LINK_TARGET" != "$BUILD_INF" ]; then
        echo "Build file isn't linked correctly: '$BUILD_TARGET_LINK_TARGET' != '$BUILD_INF'" >&2
        exit 2
    fi
fi

# dep file generation

escape_filename_for_ninja_depfile() {
    local NAME="$1"

    NAME=${NAME//\\/\\\\} # backslash
    NAME=${NAME// /\\ }   # spaces
    NAME=${NAME//#/\\#}   # #
    NAME=${NAME//$/\$\$}  # $
    printf '%s' "$NAME"

}

add_dep_file() {
    local FILE="$1"

    if ! [ -e "$FILE" ]; then
        echo "Dependency file doesn't exist: '$FILE'" >&2
        exit 2
    fi

    echo -n "$(escape_filename_for_ninja_depfile "${FILE}") " >>"$DEPS_DEST_FILE"

}

analyze_inf_file() {
    local INF_FILE="$1"

    # add inf file
    add_dep_file "$INF_FILE"

    # add sources

    local PARENT_FOLDER="$(dirname "$INF_FILE")"

    local sources

    mapfile -t sources < <(
        awk '
      /^\[Sources\]/ { in_sources=1; next }
      /^\[/          { in_sources=0 }
      in_sources {
          gsub(/^[[:space:]]+|[[:space:]]+$/, "")
          if ($0 != "" && $0 !~ /^#/)
              print
      }
    ' "$INF_FILE"
    )

    for SRC in "${sources[@]}"; do

        if [[ "${SRC:0:1}" != "/" ]]; then
            SRC="${PARENT_FOLDER}/${SRC}"
        fi

        if [[ "$SRC" =~ ^.*\$\(OPENSSL_PATH\).*$ ]]; then
            SRC="${SRC//\$(OPENSSL_PATH)/openssl}"
        fi

        if [[ "$SRC" =~ ^.*\$\(OPENSSL_GEN_PATH\).*$ ]]; then
            SRC="${SRC//\$(OPENSSL_GEN_PATH)/OpensslGen}"
        fi

        if [[ "$SRC" =~ ^.*\|.*$ ]]; then
            SRC="${SRC%%|*}"
        fi

        if [[ "$SRC" =~ ^.*#.*$ ]]; then
            SRC="${SRC%%#*}"
        fi

        SRC="${SRC%"${SRC##*[![:space:]]}"}"

        add_dep_file "$SRC"

    done

}

REPORT_FILE="$WORKSPACE/BuildReport.txt"

# generate build report
"$EDK2_BUILD_COMMAND" -a "$EDK2_TARGET_PROPERTIES_ARCH" \
    -p "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" \
    -m "$BUILD_NAME" \
    -b "$EDK2_TARGET_PROPERTIES_BUILDTYPE" \
    -t "$EDK2_TARGET_PROPERTIES_TOOLCHAIN" \
    -D "OOPETRIS_RUNTIME_TARGET=$EDK2_TARGET_PROPERTIES_RUNTIME_TARGET" \
    -n 0 \
    -w \
    -Y PCD \
    -Y LIBRARY \
    -Y FLASH \
    -Y DEPEX \
    -Y BUILD_FLAGS \
    -Y FIXED_ADDRESS \
    -Y HASH \
    -Y EXECUTION_ORDER \
    -Y COMPILE_INFO \
    -y "$REPORT_FILE"

# analyze dependencies

rm -f "$DEPS_DEST_FILE"
validate_parent_dir "$DEPS_DEST_FILE"
echo -n "$(escape_filename_for_ninja_depfile "${DEST_FILE_ORIG}"): " >"$DEPS_DEST_FILE"

line=""

analyze_state="unknown"
analyze_buffer=""

while IFS="" read -r line; do

    if [ "$analyze_state" == "unknown" ]; then
        if [[ "$line" =~ ^\>-+\<$ ]]; then
            analyze_state="line"
        fi
    elif [ "$analyze_state" == "line" ]; then
        if [[ "$line" == "Library" ]]; then
            analyze_state="libs"
        else
            analyze_state="unknown"
        fi
    elif [ "$analyze_state" == "libs" ]; then
        if [[ "$line" =~ ^\<-+\>$ ]]; then
            analyze_state="unknown"
            if [ "$analyze_buffer" != "" ]; then
                echo "Line Analyze buffer is not empty" >&2
                exit 2
            fi
        elif [[ "$line" =~ ^\{.*$ ]] || [[ "$line" =~ ^-+$ ]]; then
            # skip line
            if [ "$analyze_buffer" != "" ]; then
                echo "Line Analyze buffer is not empty" >&2
                exit 2
            fi
        else
            analyze_buffer="${analyze_buffer}${line}"

            if [[ "$analyze_buffer" =~ ^.*\.inf$ ]]; then
                analyze_inf_file "$analyze_buffer"
                analyze_buffer=""
            fi

        fi
    fi

done <"$REPORT_FILE"

analyze_inf_file "$BUILD_INF"

# final build

"$EDK2_BUILD_COMMAND" -a "$EDK2_TARGET_PROPERTIES_ARCH" \
    -p "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" \
    -m "$BUILD_NAME" \
    -b "$EDK2_TARGET_PROPERTIES_BUILDTYPE" \
    -t "$EDK2_TARGET_PROPERTIES_TOOLCHAIN" \
    -D "OOPETRIS_RUNTIME_TARGET=$EDK2_TARGET_PROPERTIES_RUNTIME_TARGET" \
    -n 0 \
    -w # -v # verbose

##TODO: un-hardcode this
##NOTE: this is just hardcoded
PACKAGE_NAME="OOPetrisApplication"

if [ "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME" == "Platforms/OOPetrisPlatform.dsc" ]; then
    MODULE_NAME="OOPetrisModule"
else
    echo "MODULE_NAME mapping not supported for platform: $EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME" >&2
    exit 2
fi

OUTPUT_FILE="$WORKSPACE/Build/$MODULE_NAME/${EDK2_TARGET_PROPERTIES_BUILDTYPE}_${EDK2_TARGET_PROPERTIES_TOOLCHAIN}/${EDK2_TARGET_PROPERTIES_ARCH}/$PACKAGE_NAME.efi"

# link the output .efi to the correct location
if ! [ -e "$DEST_FILE" ]; then
    link_files_checked "$OUTPUT_FILE" "$DEST_FILE"
fi
