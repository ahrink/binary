#!/bin/sh
set -eu
. "$(dirname "$0")/00_env.sh"

SYM_TME="⧖"  # TME marker

stamp="$(../AHRtID)"
tar_head="$(printf '%s' "$stamp" | cut -c 1-14)"
tar_file="${tar_head}${SYM_TME}SIL.tar.gz"
tar_BIOS="$stamp"

printf '%s\n' "FILE: $tar_file BIOS: $tar_BIOS"
