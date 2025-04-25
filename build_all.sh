#!/bin/bash

set -e

BUILD_TARGETS=(
  "kazoo_connect"
  "kazoo_connect_server"
)

FORCE_YES=false
if [[ "$1" == "-y" ]]; then
  FORCE_YES=true
fi

prompt() {
  if $FORCE_YES; then
    return 0
  fi
  # while true; do
    read -p "$1 (y/n) [default y]: " yn
    case $yn in
      [yY] )
        return 0
        ;;
      [nN] )
        echo "Skipping..."
        return 1
        ;;
      * )
        return 0
        # echo "Invalid input. Please answer y or n."
        ;;
    esac
  # done
}

log() {
  ARROW_COLOR="\033[33m"
  CONTENT_COLOR="\033[36m"
  RESET_CODE="\033[0m"
  echo -e "  $ARROW_COLOR>>$CONTENT_COLOR $1 $ARROW_COLOR<<$RESET_CODE"
}

# ensure we're in the right place
if [[ "$(git rev-parse --show-toplevel 2>/dev/null)" != "$(pwd)" ]]; then
  echo "Error: This script must be run from the root of the Git repository."
  exit 1
fi

log "cleaning up old builds"
for target in "${BUILD_TARGETS[@]}"; do
  rm -f "$target"
done

prompt "Do you want to install dependencies using apt?" && {
  log "installing dependencies"
  sudo apt update
  sudo apt install \
    build-essential \
    cmake \
    libgtest-dev \
    libpulse-dev \
    libfftw3-dev
}

log "building C++ project"
mkdir -p build
cd build
cmake ..
make -j$(nproc)
for target in "${BUILD_TARGETS[@]}"; do
  cp "$target" ..
done

prompt "Do you want to run tests?" && {
  log "running tests"
  ./kazoo_translation_layer_test
}

prompt "Install web client packages? (NPM packages)" && {
  log "installing web client packages"
  cd ../src/kazoo_connect_server/web_client
  npm install
}