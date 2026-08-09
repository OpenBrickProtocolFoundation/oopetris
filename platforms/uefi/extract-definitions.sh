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

expand_array_inf() {
  local FILES=("$@")

  for FILE in "${FILES[@]}"; do

    echo "  $FILE"

  done

}

is_soname() {
  local SO_NAME="$1"
  [[ "$SO_NAME" =~ (^|/)lib[^/]+\.so(\.[0-9]+)*$ ]]
}

output_stem() {
  local INPUT="$1"

  if is_soname "$INPUT"; then
    echo "${INPUT%%.so*}"
  else
    echo "${INPUT%.*}"
  fi

}

get_mapped_version() {
  local INPUT="$1"

  MESON_PROJECT_INFO_FILE="$(pwd)/meson-info/intro-projectinfo.json"

  if [ "$INPUT" == "oopetris" ]; then
    jq -M -r -c ".[\"version\"]" "$MESON_PROJECT_INFO_FILE"
  elif [ "$INPUT" == "null" ]; then
    echo "Error: invalid version argument: $INPUT" >&2
    exit 2
  else
    jq -M -r -c ".[\"subprojects\"][] | select(.[\"name\"] == \"$INPUT\") | .[\"version\"]" "$MESON_PROJECT_INFO_FILE"
  fi

}

if [ "$#" -eq 2 ]; then
  SOURCE_FILE="$1"
  DEST_FILE="$(realpath "$2")"
else
  echo "Too many arguments given, expected 2" >&2
  exit 1
fi

C_LIBRARIES=("LibC" "LibCType" "LibLocale" "LibMath" "LibSignal" "LibStdio" "LibStdLib" "LibString" "LibTime" "LibUefi" "LibWchar")
CPP_LIBRARIES=("LibCXX" "LibCXXABI")

MAPPINGS_FILE="$(realpath "$SCRIPT_DIR/../../src/executables/platforms/uefi/mappings.json")"

DEST_FILE_DIR=$(dirname -- "$DEST_FILE")
DEST_FILE_NAME=$(basename -- "$DEST_FILE")
DEST_FILE_EXT="${DEST_FILE_NAME##*.}"
DEST_FILE_STEM="${DEST_FILE_NAME%.*}"

if [ "$DEST_FILE_EXT" != "inf" ]; then
  echo "Error: invalid dest file extension: $DEST_FILE_EXT" >&2
  exit 2
fi

MAPPING_ENTRY="$(jq -M -r -c ".[\"file\"][\"${DEST_FILE_STEM}\"]" "$MAPPINGS_FILE")"

if [ "$MAPPING_ENTRY" = "null" ]; then
  echo "Error: invalid name '$DEST_FILE_STEM': not found in mappings 'file'" >&2
  exit 2
fi

SOURCE_FILE_TYPE="$(jq -M -r -c '.["type"]' "$SOURCE_FILE")"

if [ "$SOURCE_FILE_TYPE" == "link" ] || [ "$SOURCE_FILE_TYPE" == "archive" ]; then
  :
else
  echo "Error: invalid source file type: $SOURCE_FILE_TYPE" >&2
  exit 2
fi

mapfile -t FILE_DEPENDENCIES < <(jq -e '.["dependencies"]["files"][]' -M -r -c "$SOURCE_FILE")

UEFI_INFO_FILE="$(realpath "$SCRIPT_DIR/../crossbuild/uefi_info.json")"
UEFI_INFO_GENERATED_PACKAGES_DIR="$(jq -M -r -c '.["generated_packages"]' "$UEFI_INFO_FILE")"
EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="$(jq -M -r -c '.["platform"]' "$UEFI_INFO_FILE")"

add_component_to_platform() {
  local PACKAGE_NAME="$1"

  local FINAL_PKG_NAME="GeneratedPackages/$PACKAGE_NAME"

  if grep -q "$FINAL_PKG_NAME" "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"; then
    : # found already, do nothing
  else
    cat <<EOF >>"$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"
[Components]
  $FINAL_PKG_NAME

EOF

  fi

}

add_include_to_platform() {
  local PACKAGE_NAME="$1"

  local FINAL_PKG_INC_NAME="GeneratedPackages/$PACKAGE_NAME"

  if grep -q "$FINAL_PKG_INC_NAME" "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"; then
    : # found already, do nothing
  else
    cat <<EOF >>"$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"
!include $FINAL_PKG_INC_NAME

EOF

  fi

}

link_package() {
  local PACKAGE_NAME="$1"
  local SOURCE_FILE="$2"

  local FINAL_FILE="$UEFI_INFO_GENERATED_PACKAGES_DIR/$PACKAGE_NAME"

  if [ -e "$FINAL_FILE" ]; then
    rm "$FINAL_FILE"
  fi

  validate_parent_dir "$FINAL_FILE"

  link_files_checked "$SOURCE_FILE" "$FINAL_FILE"

  local SOURCE_FILE_EXT="${SOURCE_FILE##*.}"

  if [ "$SOURCE_FILE_EXT" == "inf" ]; then
    add_component_to_platform "$PACKAGE_NAME"
  elif [ "$SOURCE_FILE_EXT" == "dec" ]; then
    :
  elif [ "$SOURCE_FILE_EXT" == "inc" ]; then
    add_include_to_platform "$PACKAGE_NAME"
  else
    echo "Invalid package extension in link_package '$SOURCE_FILE_EXT'" >&2
    exit 2
  fi

}

DEP_SOURCES=()
DEP_PACKAGES=()
DEP_CLASSES=()
DEP_BUILD_OPTIONS=()

for FILE_DEPENDENCY in "${FILE_DEPENDENCIES[@]}"; do

  FILE_DEPENDENCY_TYPE="$(jq -M -r -c '.["type"]' "$FILE_DEPENDENCY")"

  if [ "$FILE_DEPENDENCY_TYPE" == "compile" ]; then
    mapfile -t FILE_DEPENDENCY_SRC < <(jq -e '.["dependencies"]["src"][]' -M -r -c "$FILE_DEPENDENCY")

    for FILE_DEPENDENCY_SRC in "${FILE_DEPENDENCY_SRC[@]}"; do
      DEP_SOURCES+=("$FILE_DEPENDENCY_SRC")
    done
  elif [ "$FILE_DEPENDENCY_TYPE" == "archive" ] || [ "$FILE_DEPENDENCY_TYPE" == "link" ]; then

    DEP_OUTPUT="$(jq -e -M -r -c '.["dependencies"]["output"]' "$FILE_DEPENDENCY")"

    if [ "$DEP_OUTPUT" = "null" ]; then
      echo "Error: output '$DEP_OUTPUT'" >&2
      exit 2
    fi

    DEP_OUTPUT_NAME=$(basename -- "$DEP_OUTPUT")
    DEP_OUTPUT_STEM="$(output_stem "$DEP_OUTPUT_NAME")"

    MAPPING_ENTRY_DEP="$(jq -M -r -c ".[\"name\"][\"${DEP_OUTPUT_STEM}\"]" "$MAPPINGS_FILE")"

    if [ "$MAPPING_ENTRY_DEP" = "null" ]; then
      echo "Error: invalid name '$DEP_OUTPUT_STEM': not found in mappings 'name' (dep)" >&2
      exit 2
    fi

    MAPPING_ENTRY_DEP_NAME="$(echo "$MAPPING_ENTRY_DEP" | jq -M -r -c ".[\"name\"]")"

    if [ "$MAPPING_ENTRY_DEP_NAME" = "null" ]; then
      echo "Error: invalid name '$MAPPING_ENTRY_DEP_NAME': not found in dependency mapping 'name'" >&2
      exit 2
    fi

    MAPPING_ENTRY_FOR_LIB="$(jq -M -r -c ".[\"file\"][\"${MAPPING_ENTRY_DEP_NAME}\"]" "$MAPPINGS_FILE")"

    if [ "$MAPPING_ENTRY_FOR_LIB" = "null" ]; then
      echo "Error: invalid name '$MAPPING_ENTRY_DEP_NAME': not found in mappings 'file'" >&2
      exit 2
    fi

    MAPPING_ENTRY_DEP_LIB="$(echo "$MAPPING_ENTRY_FOR_LIB" | jq -M -r -c ".[\"lib\"]")"

    if [ "$MAPPING_ENTRY_DEP_LIB" = "null" ]; then
      echo "Error: invalid lib name '$MAPPING_ENTRY_DEP_LIB': not found in dependency mapping 'lib'" >&2
      exit 2
    fi

    PACKAGE_NAME="GeneratedPackages/Lib/$MAPPING_ENTRY_DEP_NAME.dec"

    DEP_OUTPUT_FILE="$DEST_FILE_DIR/$MAPPING_ENTRY_DEP_NAME.inf"

    if ! [ -e "$DEP_OUTPUT_FILE" ]; then
      "$SCRIPT_DIR/extract-definitions.sh" "$FILE_DEPENDENCY" "$DEP_OUTPUT_FILE"
    fi

    DEP_PACKAGES+=("$PACKAGE_NAME")
    DEP_CLASSES+=("$MAPPING_ENTRY_DEP_LIB")
  else
    echo "Error: invalid source file type: $FILE_DEPENDENCY_TYPE" >&2
    exit 2
  fi

done

DEP_INCLUDES=()

MESON_TARGETS_INFO_FILE="$(pwd)/meson-info/intro-targets.json"

SOURCE_FILE_OUTPUT="$(jq -M -r -c '.["dependencies"]["output"]' "$SOURCE_FILE")"

if [ "$SOURCE_FILE_OUTPUT" = "null" ]; then
  echo "Error: output '$SOURCE_FILE_OUTPUT'" >&2
  exit 2
fi

SOURCE_FILE_OUTPUT_NAME=$(basename -- "$SOURCE_FILE_OUTPUT")
SOURCE_FILE_OUTPUT_STEM="$(output_stem "$SOURCE_FILE_OUTPUT_NAME")"

SOURCE_FILE_ENTRY="$(jq -M -r -c ".[\"name\"][\"${SOURCE_FILE_OUTPUT_STEM}\"]" "$MAPPINGS_FILE")"

if [ "$SOURCE_FILE_ENTRY" = "null" ]; then
  echo "Error: invalid name '$SOURCE_FILE_OUTPUT_STEM': not found in mappings 'name' (src)" >&2
  exit 2
fi

SOURCE_FILE_ENTRY_MESON_NAME="$(echo "$SOURCE_FILE_ENTRY" | jq -M -r -c ".[\"meson\"]")"

if [ "$SOURCE_FILE_ENTRY_MESON_NAME" = "null" ]; then
  echo "Error: invalid name '$SOURCE_FILE_ENTRY_MESON_NAME': not found in dependency mapping 'meson'" >&2
  exit 2
fi

MESON_TARGETS_INFO_LIB="$(
  jq -M -r -c ".[] | select(.[\"name\"] == \"$SOURCE_FILE_ENTRY_MESON_NAME\")" "$MESON_TARGETS_INFO_FILE"
)"

if [ "$MESON_TARGETS_INFO_LIB" = "null" ]; then
  echo "Error: invalid name '$SOURCE_FILE_ENTRY_MESON_NAME': not found in meson targets file'" >&2
  exit 2
fi

MESON_TARGETS_INFO_LIB_TYPE="$(echo "$MESON_TARGETS_INFO_LIB" | jq -M -r -c ".[\"type\"]")"

mapfile -t MESON_TARGETS_INFO_LIB_COMPILER_TARGETS < <(echo "$MESON_TARGETS_INFO_LIB" | jq '.["target_sources"][] | select( has("compiler") )' -M -r -c)

if [ "${#MESON_TARGETS_INFO_LIB_COMPILER_TARGETS[@]}" -ne 1 ]; then
  echo "Array is not one element long but ${#MESON_TARGETS_INFO_LIB_COMPILER_TARGETS[@]} long" >&2
  printf '%s\n' "${MESON_TARGETS_INFO_LIB_COMPILER_TARGETS[@]}" >&2
  exit 9
fi

MESON_TARGETS_INFO_LIB_COMPILER_TARGET="${MESON_TARGETS_INFO_LIB_COMPILER_TARGETS[0]}"

MESON_TARGETS_INFO_LIB_LANGUAGE="$(echo "$MESON_TARGETS_INFO_LIB_COMPILER_TARGET" | jq -M -r -c ".[\"language\"]")"

FLAGS_TARGET="CC"

if [ "$MESON_TARGETS_INFO_LIB_LANGUAGE" == "cpp" ]; then
  FLAGS_TARGET="CXX"
  for CPP_LIBRARY in "${CPP_LIBRARIES[@]}"; do
    DEP_CLASSES+=("$CPP_LIBRARY")
  done
elif [ "$MESON_TARGETS_INFO_LIB_LANGUAGE" == "c" ]; then
  FLAGS_TARGET="CC"
else
  echo "Error: invalid meson target language: $MESON_TARGETS_INFO_LIB_LANGUAGE" >&2
  exit 2
fi

mapfile -t MESON_TARGETS_INFO_LIB_PARAMATERS < <(echo "$MESON_TARGETS_INFO_LIB_COMPILER_TARGET" | jq '.["parameters"][]' -M -r -c)

for MESON_TARGETS_INFO_LIB_PARAMATER in "${MESON_TARGETS_INFO_LIB_PARAMATERS[@]}"; do

  case "$MESON_TARGETS_INFO_LIB_PARAMATER" in
  -I*)
    DEP_INCLUDES+=("${MESON_TARGETS_INFO_LIB_PARAMATER:2}")
    ;;
  -W*)
    # TODO: include warnings
    ;;
  -nostdinc | -nostdinc++ | -nodefaultlibs | -D* | --sysroot=* | -isystem*)
    DEP_BUILD_OPTIONS+=("*_*_*_${FLAGS_TARGET}_FLAGS = \"${MESON_TARGETS_INFO_LIB_PARAMATER}\"")
    ;;
  -t:use-lib:pkg=*)
    DEP_PACKAGE_NAME="${MESON_TARGETS_INFO_LIB_PARAMATER:15}"
    DEP_PACKAGES+=("LibraryPkg/$DEP_PACKAGE_NAME/$DEP_PACKAGE_NAME.dec")
    ;;
  -t:use-pkg:pkg=*)
    DEP_PACKAGE_NAME="${MESON_TARGETS_INFO_LIB_PARAMATER:15}"
    DEP_PACKAGES+=("$DEP_PACKAGE_NAME")
    ;;
  -t:use-lib:name=* | -t:use-pkg:name=*)
    #ignore
    ;;
  -t:use-lib:lib=*)
    DEP_PACKAGE_LIB="${MESON_TARGETS_INFO_LIB_PARAMATER:15}"
    DEP_CLASSES+=("$DEP_PACKAGE_LIB")
    ;;
  -t:use-pkg:lib=*)
    DEP_PACKAGE_LIB="${MESON_TARGETS_INFO_LIB_PARAMATER:15}"
    DEP_CLASSES+=("$DEP_PACKAGE_LIB")
    ;;
  -pthread)
    # ignore pthread
    ;;
  -f* | -m*)
    DEP_BUILD_OPTIONS+=("*_*_*_${FLAGS_TARGET}_FLAGS = \"${MESON_TARGETS_INFO_LIB_PARAMATER}\"")
    ;;
  -g | -std=* | -O*)
    #ignore
    ;;
  *)
    echo "Invalid argument detected for compiling '$SOURCE_FILE_ENTRY_MESON_NAME': '$MESON_TARGETS_INFO_LIB_PARAMATER'" >&2
    exit 6
    ;;
  esac
done

# TODO: what should i do here, archives alias static_libraries don't specify argument, but executables and shared libraries hae -Wl -l or similar arguments
if [ "$MESON_TARGETS_INFO_LIB_TYPE" == "static library" ]; then
  echo "TODO"
elif [ "$MESON_TARGETS_INFO_LIB_TYPE" == "executable" ] || [ "$MESON_TARGETS_INFO_LIB_TYPE" == "shared library" ]; then
  echo "TODO"
  # exit 34
else
  echo "Error: invalid meson target type for '$SOURCE_FILE_ENTRY_MESON_NAME': $MESON_TARGETS_INFO_LIB_TYPE" >&2
  exit 2
fi

MAPPING_TYPE="$(echo "$MAPPING_ENTRY" | jq -M -r -c ".[\"type\"]")"
MAPPING_NAME="$(echo "$MAPPING_ENTRY" | jq -M -r -c ".[\"name\"]")"
MAPPING_GUID="$(echo "$MAPPING_ENTRY" | jq -M -r -c ".[\"guid\"]")"
MAPPING_VERSION="$(get_mapped_version "$(echo "$MAPPING_ENTRY" | jq -M -r -c ".[\"version\"]")")"

if [ -z "$MAPPING_VERSION" ]; then
  echo "Error: invalid version extracted '$MAPPING_NAME': $MAPPING_VERSION" >&2
  exit 2

fi

if [ "$MAPPING_TYPE" == "application" ]; then

  # add include directories to the build options (as the files build inside the application need that)
  for DEP_INCLUDE in "${DEP_INCLUDES[@]}"; do
    DEP_BUILD_OPTIONS+=("*_*_*_${FLAGS_TARGET}_FLAGS = \"-I${DEP_INCLUDE}\"")
  done

  validate_parent_dir "$DEST_FILE"

  cat <<EOF >"$DEST_FILE"
[Defines]
  INF_VERSION = 1.25
  BASE_NAME = $MAPPING_NAME
  FILE_GUID = $MAPPING_GUID
  MODULE_TYPE = UEFI_APPLICATION
  VERSION_STRING = $MAPPING_VERSION
  ENTRY_POINT = UefiMain

[Sources]
$(expand_array_inf "${DEP_SOURCES[@]}")

[Packages]
  MdePkg/MdePkg.dec
  StdLib/StdLib.dec
$(expand_array_inf "${DEP_PACKAGES[@]}")

[LibraryClasses]
  UefiApplicationEntryPoint
  UefiLib
$(expand_array_inf "${C_LIBRARIES[@]}")
$(expand_array_inf "${DEP_CLASSES[@]}")

[Guids]

[Ppis]

[Protocols]

[FeaturePcd]

[Pcd]

[BuildOptions]
  *_*_*_CCONLY_FLAGS  = -std=c11
  *_*_*_CXX_FLAGS = -std=c++23

$(expand_array_inf "${DEP_BUILD_OPTIONS[@]}")
EOF

  link_package "$DEST_FILE_NAME" "$DEST_FILE"

elif [ "$MAPPING_TYPE" == "library" ]; then

  # add include directories to the build options (as the files build inside the library need that)
  # NOTE: we also define these as [Include], which is not entirely correct, but it didn't lead to problems until now
  # TODO: ^ these should be private include dirs not public ones
  for DEP_INCLUDE in "${DEP_INCLUDES[@]}"; do
    DEP_BUILD_OPTIONS+=("*_*_*_${FLAGS_TARGET}_FLAGS = \"-I${DEP_INCLUDE}\"")
  done

  # add custom options, that override fix dependencies
  MAPPING_OPTIONS="$(echo "$MAPPING_ENTRY" | jq -M -r -c ".[\"options\"]")"

  if [ "$MAPPING_OPTIONS" != "null" ]; then
    mapfile -t MAPPING_OPTIONS_ENTRIES < <(echo "$MAPPING_OPTIONS" | jq -e '.[]' -M -r -c)

    for MAPPING_OPTIONS_ENTRY in "${MAPPING_OPTIONS_ENTRIES[@]}"; do
      DEP_BUILD_OPTIONS+=("*_*_*_${FLAGS_TARGET}_FLAGS = \"${MAPPING_OPTIONS_ENTRY}\"")
    done
  fi

  MAPPING_LIB_NAME="$(echo "$MAPPING_ENTRY" | jq -M -r -c ".[\"lib\"]")"

  LIB_DEST_DIR="$DEST_FILE_DIR/Lib"

  validate_parent_dir "$LIB_DEST_DIR/dummy"

  LIB_DEST_FILE_INF="$LIB_DEST_DIR/$DEST_FILE_STEM.inf"

  cat <<EOF >"$LIB_DEST_FILE_INF"
[Defines]
  INF_VERSION = 1.25
  BASE_NAME = $MAPPING_NAME
  FILE_GUID = $MAPPING_GUID
  MODULE_TYPE = BASE
  VERSION_STRING = $MAPPING_VERSION
  LIBRARY_CLASS = $MAPPING_LIB_NAME|UEFI_APPLICATION 

[Sources]
$(expand_array_inf "${DEP_SOURCES[@]}")

[Packages]
  MdePkg/MdePkg.dec
  StdLib/StdLib.dec
$(expand_array_inf "${DEP_PACKAGES[@]}")

[LibraryClasses]
  UefiLib
$(expand_array_inf "${C_LIBRARIES[@]}")
$(expand_array_inf "${CPP_LIBRARIES[@]}")
$(expand_array_inf "${DEP_CLASSES[@]}")

[BuildOptions]
  *_*_*_CCONLY_FLAGS  = -std=c11
  *_*_*_CXX_FLAGS = -std=c++23

$(expand_array_inf "${DEP_BUILD_OPTIONS[@]}")
EOF

  link_package "Lib/$DEST_FILE_STEM.inf" "$LIB_DEST_FILE_INF"

  LIB_DEST_FILE_DEC="$LIB_DEST_DIR/$DEST_FILE_STEM.dec"

  cat <<EOF >"$LIB_DEST_FILE_DEC"
[Defines]
  DEC_SPECIFICATION   = 1.25
  PACKAGE_NAME                   = $MAPPING_NAME
  PACKAGE_GUID                   = $MAPPING_GUID
  PACKAGE_VERSION                = $MAPPING_VERSION

[Includes]
$(expand_array_inf "${DEP_INCLUDES[@]}")

[LibraryClasses]
# None # TODO, also add dsc? with ,.inf mappings
EOF

  link_package "Lib/$DEST_FILE_STEM.dec" "$LIB_DEST_FILE_DEC"

  LIB_DEST_FILE_INC="$LIB_DEST_DIR/$DEST_FILE_STEM.inc"

  cat <<EOF >"$LIB_DEST_FILE_INC"
[LibraryClasses.common]
  $MAPPING_LIB_NAME|GeneratedPackages/Lib/$DEST_FILE_STEM.inf
EOF

  link_package "Lib/$DEST_FILE_STEM.inc" "$LIB_DEST_FILE_INC"

else
  echo "Error: invalid mapping type: $MAPPING_TYPE" >&2
  exit 2
fi
