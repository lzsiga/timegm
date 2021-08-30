/* julian-timegm.h */

#ifndef JULIAN_TIMEGM_H
#define JULIAN_TIMEGM_H

/* proleptic Julian calendar: https://en.wikipedia.org/wiki/Proleptic_Julian_calendar */
/* examples:
   PJC         PGC
   ---         ---
   1969-12-19  1970-01-01
   1234-01-01  1234-01-08
    201-01-01   201-01-01
  -4712-01-01 -4713-11-24 (Julian day (JDN) starts on this day 12:00 UTC)
 */

#include <stdint.h>
#include <time.h>

time_t     julian_timegm   (struct tm *tm);
struct tm *julian_gmtime_r (const time_t *t, struct tm *tm);

/* Julian day: https://en.wikipedia.org/wiki/Julian_day
   it can be represented via struct 'JulianDate' or a floating point
   number (of type 'double'), where the integer part is the day number, eg.:

    JulianDate    double        PJC                 PGC
    ----------    ---------     ------------------- -------------------
    1721423,43200 1721423.5      1CE-01-01 00:00:00 1BCE-01-01 00:00:00
    2451923,43199 2451923.49999 2000-12-31 23:59:59 2001-01-13 23:59:59
 */

#define JDATE_START_UNIXTS (-210866760000LL) /* Julian Date 0.0 in Unix Timestamp */

typedef struct JulianDate {
    int32_t jd_day; /* day 0 starts at PJC -4712-01-01 12:00:00 UTC i.e. PGC -4713-11-24 12:00:00 UTC */
    int32_t jd_sec; /* seconds in day 0..86399 (other values result undefined behavior (UB)) */
} JulianDate;

int julian_jd2d (const JulianDate *jd, double *d);
int julian_d2jd (double d, JulianDate *jd);

int julian_tstamp2jd (int64_t unixts, JulianDate *jd);
int julian_jd2tstamp (const JulianDate *jd, int64_t *unixts);

int julian_tstamp2d (int64_t unixts, double *d);
int julian_d2tstamp (double d, int64_t *unixts);

#endif
