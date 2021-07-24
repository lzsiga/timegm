/* julian-timegm.h */

#ifndef JULIAN_TIMEGM_H
#define JULIAN_TIMEGM_H

/* proleptic Julian calendar: https://en.wikipedia.org/wiki/Proleptic_Julian_calendar */
/* examples:
   PJD         PGD
   ---         ---
   1969-12-19  1970-01-01
   1234-01-01  1234-01-08
    201-01-01   201-01-01
  -4712-01-01 -4713-11-24 (Julian day (JDN) starts on this day 12:00 UTC)
 */

#include <time.h>

time_t     julian_timegm   (struct tm *tm);
struct tm *julian_gmtime_r (const time_t *t, struct tm *tm);

#endif
