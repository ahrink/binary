#!/bin/sh
SYM_TME="⧖" # \tmedlm, tme (Time Machine Efficiency)
stamp=$(../AHRtID)
# stamp="20260206125514881676952"
echo "$stamp"
# ------output: 20260206125514881676952
# segmentation: 12345678901234
tar_file=$(echo "$stamp" | cut -c 1-14)
tar_file="${tar_file}${SYM_TME}SIL.tar.gz"
tar_BIOS="$stamp"

echo "FILE: $tar_file BIOS: $tar_BIOS"
