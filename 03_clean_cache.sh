#!/bin/sh
set -eu
. "$(dirname "$0")/00_env.sh"

# Safe: only our namespace, never /tmp/*RTC
rm -f "$AHR_TMPDIR"/AHRtID_*
