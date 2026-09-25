#!/usr/bin/env bash
# Build and launch in the local emulator
set -euo pipefail
cd "$(dirname "$0")"
./Make.sh
exec "${VIRCON32_EMULATOR:-../tools/Emulator/Vircon32}" "$(pwd)/bin/PixelHustler.v32"
