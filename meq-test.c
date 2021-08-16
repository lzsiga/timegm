/* meq-test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "julian-timegm.h"

/* source:
   https://astronomy.stackexchange.com/questions/43283/accuracy-of-calculating-the-vernal-equinox/43284#43339
 */

static void DoOne (int y);

int main (int argc, char **argv) {
    int i;
    int step= 1;

    if (argc>=2 && strcasecmp (argv[1], "-l")==0) { step= 4; }
    for (i=-1000; i<=2100; i += step) {
        DoOne (i);
    }
    return 0;
}

static void DoOne (int y) {
static const double jd0 = 2451545.0;
static const char tfmt[] = "%Y-%m-%d.%H:%M";
    int dy;
    double jd;
    int64_t ts;
    struct tm jtm, gtm;
    char sj [32];
    char sg [32];

    dy = y - 2000;
    jd = jd0 + 78.814 +
        ((((-4.5e-16)* dy - 2.87e-12)*dy + 5.004e-8)*dy + 365.24236)*dy;
    julian_d2tstamp (jd, &ts);

    {
       time_t t= ts;

       julian_gmtime_r (&t, &jtm);
       gmtime_r (&t, &gtm);
    }

    strftime (sg, sizeof sg, tfmt, &gtm);
    strftime (sj, sizeof sj, tfmt, &jtm);
    printf ("%6d: PJC: %16s  PGC: %16s\n", y, sj, sg);
}
