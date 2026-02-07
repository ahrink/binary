#ifndef AHR_TID_H
#define AHR_TID_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AHR_TID_LEN 24 /* 23 digits + NUL */

/*
  AHR naming law:
    <STAMP><⧖>PATTERN<⧖>G<GUARD><⧖>E<EPOCH><⧖>N<NSEC><⧖>M<MONO_NS>

  - STAMP: 23-digit tID (YYYYMMDDhhmmss + 9 ns digits)
  - PATTERN: semantic marker, default "⧖RTC"
  - GUARD: alpha-hextet token known to the compiled code (namespace without a prefix)
*/

/* Semantic pattern (meaning) */
#define AHR_MARK "⧖RTC"

/* GUARD: compile-time alpha-hextet token (namespace).
   Override at build time:
     make CPPFLAGS='-Iincludes -DAHR_GUARD="\"A1f4\""' */
#ifndef AHR_GUARD
#define AHR_GUARD "A0001"
#endif

/* Default: 10 seconds in nanoseconds */
#define AHR_DEFAULT_INTERVAL_NS 10000000000ULL

typedef struct {
  char     tid[AHR_TID_LEN];
  int      used_real;      /* 1 if REAL refreshed, 0 if MONO-derived */
  uint64_t mono_age_ns;    /* age of cache at call time */
} ahr_tid_result_t;

/* Core gate: REAL only once per interval, else MONO-derived based on cached REAL anchor. */
int ahr_tid_gate(ahr_tid_result_t *out,
                 int use_utc,
                 uint64_t interval_ns,
                 const char *tmp_dir);

/* Purge cache files belonging to this compiled tool (requires AHR_GUARD match). */
int ahr_tid_cache_purge(const char *tmp_dir);

#ifdef __cplusplus
}
#endif

#endif /* AHR_TID_H */
