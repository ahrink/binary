#define _POSIX_C_SOURCE 200809L

#include "ahr_tid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  /* UTC default; allow local only if explicit */
  int use_utc = 1;
  const char *use_local = getenv("AHR_LOCALTIME");
  if (use_local && strcmp(use_local, "1") == 0) use_utc = 0;

  /* interval override (ns) */
  uint64_t interval = AHR_DEFAULT_INTERVAL_NS;
  const char *ival = getenv("AHR_RTC_INTERVAL_NS");
  if (ival && *ival) {
    interval = (uint64_t)strtoull(ival, NULL, 10);
    if (interval == 0) interval = AHR_DEFAULT_INTERVAL_NS;
  }

  const char *tmp_dir = getenv("AHR_TMPDIR");
  if (!tmp_dir || !*tmp_dir) tmp_dir = "/tmp";

  ahr_tid_result_t r;
  if (ahr_tid_gate(&r, use_utc, interval, tmp_dir) != 0) return 1;

  /* Contract: print only the 23-digit stamp */
  puts(r.tid);
  return 0;
}
