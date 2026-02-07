#define _POSIX_C_SOURCE 200809L

#include "ahr_tid.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>   /* open() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* -------- helpers -------- */

static uint64_t mono_now_ns(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0;
  return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

static int fmt_tid_from_tm(char out[AHR_TID_LEN], const struct tm *t, long nsec) {
  int n = snprintf(out, AHR_TID_LEN,
                   "%04d%02d%02d%02d%02d%02d%09ld",
                   t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                   t->tm_hour, t->tm_min, t->tm_sec,
                   nsec);
  return (n == 23) ? 0 : -1;
}

static int real_now(time_t *sec_out, long *nsec_out, int use_utc, char tid_out[AHR_TID_LEN]) {
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) return -1;

  struct tm tmv;
  if (use_utc) {
    if (!gmtime_r(&ts.tv_sec, &tmv)) return -1;
  } else {
    if (!localtime_r(&ts.tv_sec, &tmv)) return -1;
  }

  if (fmt_tid_from_tm(tid_out, &tmv, ts.tv_nsec) != 0) return -1;

  if (sec_out)  *sec_out  = ts.tv_sec;
  if (nsec_out) *nsec_out = ts.tv_nsec;
  return 0;
}

static int fmt_from_epoch(time_t sec, long nsec, int use_utc, char tid_out[AHR_TID_LEN]) {
  struct tm tmv;
  if (use_utc) {
    if (!gmtime_r(&sec, &tmv)) return -1;
  } else {
    if (!localtime_r(&sec, &tmv)) return -1;
  }
  return fmt_tid_from_tm(tid_out, &tmv, nsec);
}

static int starts_with_23_digits(const char *s) {
  if (!s) return 0;
  for (int i = 0; i < 23; i++) {
    if (!isdigit((unsigned char)s[i])) return 0;
  }
  return 1;
}

/*
  Cache filename grammar (AHR law, no prefix):

    <STAMP>⧖RTC⧖G<GUARD>⧖E<EPOCH>⧖N<NSEC>⧖M<MONO_NS>

  MARK (⧖RTC) = meaning
  GUARD        = namespace known to compiled code
*/
static int is_our_cache_name(const char *name) {
  if (!name) return 0;
  if (!starts_with_23_digits(name)) return 0;

  if (!strstr(name, AHR_MARK)) return 0;

  /* Must include guard exactly (namespace) */
  char guard_tag[128];
  snprintf(guard_tag, sizeof(guard_tag), "⧖G%s", AHR_GUARD);
  if (!strstr(name, guard_tag)) return 0;

  /* Must include the required fields */
  if (!strstr(name, "⧖E")) return 0;
  if (!strstr(name, "⧖N")) return 0;
  if (!strstr(name, "⧖M")) return 0;

  return 1;
}

static int parse_cache_name(const char *name,
                            char stamp_out[AHR_TID_LEN],
                            time_t *epoch_out,
                            long *nsec_out,
                            uint64_t *mono_base_out) {
  if (!is_our_cache_name(name)) return -1;

  char stamp[AHR_TID_LEN] = {0};
  memcpy(stamp, name, 23);
  stamp[23] = '\0';

  const char *e = strstr(name, "⧖E");
  const char *n = strstr(name, "⧖N");
  const char *m = strstr(name, "⧖M");
  if (!e || !n || !m) return -1;

  long long epoch_ll = 0;
  long nsec_l = 0;
  unsigned long long mono_ull = 0;

  if (sscanf(e, "⧖E%lld", &epoch_ll) != 1) return -1;
  if (sscanf(n, "⧖N%ld", &nsec_l) != 1) return -1;
  if (sscanf(m, "⧖M%llu", &mono_ull) != 1) return -1;

  if (nsec_l < 0 || nsec_l > 999999999L) return -1;

  if (stamp_out) memcpy(stamp_out, stamp, AHR_TID_LEN);
  if (epoch_out) *epoch_out = (time_t)epoch_ll;
  if (nsec_out)  *nsec_out  = nsec_l;
  if (mono_base_out) *mono_base_out = (uint64_t)mono_ull;

  return 0;
}

static int find_existing_cache(const char *tmp_dir, char *name_out, size_t name_out_sz) {
  DIR *d = opendir(tmp_dir);
  if (!d) return -1;

  struct dirent *de;
  while ((de = readdir(d)) != NULL) {
    if (is_our_cache_name(de->d_name)) {
      strncpy(name_out, de->d_name, name_out_sz - 1);
      name_out[name_out_sz - 1] = '\0';
      closedir(d);
      return 0;
    }
  }
  closedir(d);
  return -1;
}

static int write_cache_file(const char *tmp_dir, const char *cache_name) {
  char path[4096];
  snprintf(path, sizeof(path), "%s/%s", tmp_dir, cache_name);

  /* open is explicit and declared in <fcntl.h> */
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (fd < 0) return -1;
  close(fd);
  return 0;
}

/* -------- public -------- */

int ahr_tid_cache_purge(const char *tmp_dir) {
  if (!tmp_dir) return -1;

  DIR *d = opendir(tmp_dir);
  if (!d) return -1;

  struct dirent *de;
  while ((de = readdir(d)) != NULL) {
    if (is_our_cache_name(de->d_name)) {
      char path[4096];
      snprintf(path, sizeof(path), "%s/%s", tmp_dir, de->d_name);
      (void)unlink(path);
    }
  }

  closedir(d);
  return 0;
}

int ahr_tid_gate(ahr_tid_result_t *out,
                 int use_utc,
                 uint64_t interval_ns,
                 const char *tmp_dir) {
  if (!out || !tmp_dir) return -1;
  memset(out, 0, sizeof(*out));

  if (interval_ns == 0) interval_ns = AHR_DEFAULT_INTERVAL_NS;

  /* developer override (explicit) */
  const char *force_real = getenv("AHR_FORCE_REAL");
  if (force_real && strcmp(force_real, "1") == 0) {
    time_t sec; long nsec;
    if (real_now(&sec, &nsec, use_utc, out->tid) != 0) return -1;

    uint64_t mono_base = mono_now_ns();
    (void)ahr_tid_cache_purge(tmp_dir);

    char cache_name[768];
    snprintf(cache_name, sizeof(cache_name),
             "%s%s⧖G%s⧖E%lld⧖N%ld⧖M%llu",
             out->tid, AHR_MARK, AHR_GUARD,
             (long long)sec, nsec,
             (unsigned long long)mono_base);

    (void)write_cache_file(tmp_dir, cache_name);

    out->used_real = 1;
    out->mono_age_ns = 0;
    return 0;
  }

  /* Try cache */
  char found[768] = {0};
  char cached_stamp[AHR_TID_LEN] = {0};
  time_t cached_epoch = 0;
  long cached_nsec = 0;
  uint64_t cached_mono_base = 0;

  int have_cache =
      (find_existing_cache(tmp_dir, found, sizeof(found)) == 0) &&
      (parse_cache_name(found, cached_stamp, &cached_epoch, &cached_nsec, &cached_mono_base) == 0);

  uint64_t mono_now = mono_now_ns();
  uint64_t age = 0;
  if (have_cache && mono_now >= cached_mono_base) age = mono_now - cached_mono_base;

  /* Refresh REAL if missing/expired */
  if (!have_cache || age >= interval_ns) {
    time_t sec; long nsec;
    if (real_now(&sec, &nsec, use_utc, out->tid) != 0) return -1;

    uint64_t mono_base = mono_now_ns();
    (void)ahr_tid_cache_purge(tmp_dir);

    char cache_name[768];
    snprintf(cache_name, sizeof(cache_name),
             "%s%s⧖G%s⧖E%lld⧖N%ld⧖M%llu",
             out->tid, AHR_MARK, AHR_GUARD,
             (long long)sec, nsec,
             (unsigned long long)mono_base);

    (void)write_cache_file(tmp_dir, cache_name);

    out->used_real = 1;
    out->mono_age_ns = 0;
    return 0;
  }

  /* MONO-derived from cached REAL anchor */
  out->mono_age_ns = age;

  time_t sec = cached_epoch + (time_t)(age / 1000000000ULL);
  long nsec = cached_nsec + (long)(age % 1000000000ULL);
  if (nsec >= 1000000000L) {
    sec += 1;
    nsec -= 1000000000L;
  }

  if (fmt_from_epoch(sec, nsec, use_utc, out->tid) != 0) return -1;

  out->used_real = 0;
  return 0;
}
