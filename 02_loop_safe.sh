#!/bin/sh
set -eu
. "$(dirname "$0")/00_env.sh"

# Intentionally call in a loop: RTC will NOT be abused.
i=0
while [ "$i" -lt 8 ]; do
  stamp="$(../AHRtID)"
  printf '%s\n' "$stamp"
  i=$((i+1))
done
