/* aix-timegm-test.c */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define timegm aix_timegm
#include "aix-timegm.c"
#undef timegm

static int Test1 (const struct tm *ptm, int offsyear);

int main (void)
{
    time_t tnow;
    struct tm tm;
    char ts[32];

    time (&tnow);
    memset (&tm, 0, sizeof tm);
    gmtime_r (&tnow, &tm);
    strftime (ts, sizeof ts, "%Y-%m-%d %H:%M:%S", &tm);

    Test1 (&tm, 0);
#if LONG_MAX > 2147483647L
    {
	int i;

	for (i= -2000; i<2000; i += 93) {
	    Test1 (&tm, i);
	}
    }
#endif
    return 0;
}

static int Test1 (const struct tm *ptm, int offsyear)
{
    struct tm tm= *ptm;
    time_t tglibc, town;

    tm.tm_year += offsyear;
    tglibc= timegm (&tm);
    town= aix_timegm (&tm);
    fprintf (stderr, "test for %04d-%02d-%02d: glibc=%12lld own=%12lld (delta=%lld)\n",
	tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
	(long long)tglibc, (long long)town, (long long)(tglibc-town));
    return 0;
}
