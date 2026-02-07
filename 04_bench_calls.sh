#!/bin/sh
set -eu
. "$(dirname "$0")/00_env.sh"

# Show: many calls, only one REAL refresh per interval.
# We infer “refresh” by watching when the /tmp file changes name.

before="$(ls -1 "$AHR_TMPDIR"/AHRtID_* 2>/dev/null | head -n 1 || true)"
printf '%s\n' "CACHE_BEFORE: ${before:-none}"

n=0
while [ "$n" -lt 20 ]; do
  ../AHRtID >/dev/null
  n=$((n+1))
done

after="$(ls -1 "$AHR_TMPDIR"/AHRtID_* 2>/dev/null | head -n 1 || true)"
printf '%s\n' "CACHE_AFTER:  ${after:-none}"
