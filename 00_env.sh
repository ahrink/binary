#!/bin/sh
set -eu

# Default policy:
# - UTC output
# - REAL refresh allowed once per 10s, otherwise MONO-derived
: "${AHR_TMPDIR:=/tmp}"
: "${AHR_RTC_INTERVAL_NS:=10000000000}"   # 10 seconds in ns
export AHR_TMPDIR AHR_RTC_INTERVAL_NS

echo "$AHR_RTC_INTERVAL_NS"
echo "$AHR_TMPDIR"

# Reset ONLY this compiled tool’s cache (guarded)
# rm -f /tmp/*⧖RTC⧖G"A0001"⧖E*⧖N*⧖M*
