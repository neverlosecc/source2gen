#!/bin/bash

set -Eueo pipefail

GAME_DIRECTORY="${1:-}"
SCRIPT_DIRECTORY="$(dirname "$(readlink -f "$0")")"
PROJECT_ROOT="${SCRIPT_DIRECTORY}/../"
BINARY=""

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
  set -x
  LD_LIBRARY_PATH="${GAME_DIRECTORY}/game/bin/linuxsteamrt64/:${GAME_DIRECTORY}/game/csgo/bin/linuxsteamrt64/:${LD_LIBRARY_PATH:-}" "${BINARY}"
fi
