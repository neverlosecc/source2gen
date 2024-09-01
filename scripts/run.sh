#!/bin/bash

set -Eueo pipefail

GAME_DIRECTORY="${1:-}"
SCRIPT_DIRECTORY="$(dirname "$(readlink -f "$0")")"
PROJECT_ROOT="${SCRIPT_DIRECTORY}/../"
BINARY=""

find_second_bin_directory() {
    local game_path="$1"

    local found;found="$(find "${game_path}" -name libclient.so)"

    if [ -z "${found}" ]; then
      echo "Error: unable to find second bin directory" >&2
      exit 1
    else
      dirname "${found}"
    fi
}

if [ -z "${GAME_DIRECTORY}" ]; then
  echo "usage: run.sh <path/to/game/directory>"
  echo "eg   : run.sh $HOME/.steam/steam/steamapps/cs2/"
  exit 1
fi

for subdirectory in \
  "build/bin/source2gen" \
  "build/debug/bin/source2gen" \
  "build/release/bin/source2gen"
  do
  target="${PROJECT_ROOT}/${subdirectory}"
  if [ -f "${target}" ]; then
    BINARY="${target}"
    break
  fi
done

if [ -z "${BINARY}" ]; then
  echo "source2gen binary not found. set LD_PRELOAD_PATH and run source2gen by hand."
  exit 1
else
  FIRST_BIN_DIRECTORY="${GAME_DIRECTORY}/game/bin/linuxsteamrt64/"
  SECOND_BIN_DIRECTORY=$(find_second_bin_directory "$GAME_DIRECTORY")
  export LD_LIBRARY_PATH="${FIRST_BIN_DIRECTORY}:${SECOND_BIN_DIRECTORY}:${LD_LIBRARY_PATH:-}"
  set -x
  if [ -z "${DEBUGGER:-}" ]; then
    "${BINARY}"
  else
    "${DEBUGGER}" -- "${BINARY}"
  fi
fi
