/* aix-timegm-test.c */

#include <stdarg.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* include the source: it's just for the test, not an example for normal usage */
#define timegm aix_timegm
#include "aix-timegm.c"
#undef timegm

static int Test1 (const struct tm *ptm, int offsyear);
static int Test0 (const struct tm *ptm, const char *fmt, ...);
static int Test2 (int y, int m, int d);

int main (void)
{
    time_t tnow;
    struct tm tm;

    time (&tnow);
    memset (&tm, 0, sizeof tm);
    gmtime_r (&tnow, &tm);

    Test2 (1600, 12, 31);

    Test1 (&tm, 0);
#if LONG_MAX > 2147483647L
    {
	int i;

	for (i= -2000; i<2000; i += 93) {
	    Test1 (&tm, i);
	}
    }
    {
	int i;

	for (i= 0; i<2000; ++i) {
	    Test2 (i,  2, 29);
	    Test2 (i, 12, 31);
	    Test2 (i+1,  1,  1);
	}
    }
#endif

    return 0;
}

static int Test1 (const struct tm *ptm, int offsyear)
{
    struct tm tm= *ptm;

    tm.tm_year += offsyear;
    Test0 (&tm, "test for %04d-%02d-%02d: ",
	tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
    return 0;
}

static int Test2 (int y, int m, int d)
{
    struct tm tm;

    memset (&tm, 0, sizeof tm);
    tm.tm_year= y-1900;
    tm.tm_mon= m-1;
    tm.tm_mday= d;

    Test0 (&tm, "test for %04d-%02d-%02d: ",
	tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
    return 0;
}

static int Test0 (const struct tm *ptm, const char *fmt, ...)
{
    time_t tglibc, town;
    va_list ap;
    struct tm tm1= *ptm;
    struct tm tm2= *ptm;

    va_start (ap, fmt);
    vfprintf (stderr, fmt, ap);

    tglibc= timegm (&tm1);
    town= aix_timegm (&tm2);

    fprintf (stderr, "glibc=%12lld own=%12lld",
	(long long)tglibc, (long long)town);

    if (tglibc != town) {
	fprintf (stderr, " *** delta=%lld", (long long)(tglibc-town));
    } else {
	fprintf (stderr, " delta=0");
    }

    if (tm1.tm_wday != tm2.tm_wday) {
        fprintf (stderr, " wday=%d/%d", tm1.tm_wday, tm2.tm_wday);
    }
    if (tm1.tm_yday != tm2.tm_yday) {
        fprintf (stderr, " yday=%d/%d", tm1.tm_yday, tm2.tm_yday);
    }
    if (tm1.tm_isdst != tm2.tm_isdst) {
        fprintf (stderr, " isdst=%d/%d", tm1.tm_isdst, tm2.tm_isdst);
    }
    fputc ('\n', stderr);
    return 0;
}
