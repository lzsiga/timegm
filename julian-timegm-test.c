/* julian_timegm_test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "julian-timegm.h"

static void Test1 (time_t t);
static void Test2 (int y, int m, int d);
static void Test3 (int y, int m, int d, int H, int M, int S);

static void Test4 (int jday, int jsec);

int main (void) {
    int i;

    Test1 (0); /* PGC 1970-01-01, PJC 1969-12-19 */
    for (i=1; i<24; ++i) Test1 (i*50*60+i);
    for (i=1; i<24; ++i) Test1 (i*24L*3600);
    Test2 ( 2021,  7, 11);
    Test2 (  201,  1,  1);
    Test2 (  204,  2, 28);
    Test2 (  204,  2, 29);
    Test2 (  204,  3,  1);
    Test2 (  299, 12, 31);
    Test2 (-4712,  1,  1);           /* 4713BC Jan 1st midnight */
    Test3 (-4712,  1,  1, 12, 0, 0); /* 4713BC Jan 1st noon */

    Test4 (2456293, 45000);
    Test4 (      0,     0);
    return 0;
}

static void Test1 (time_t t) {
static const char tfmt[] = "%Y-%m-%d.%H:%M:%S_%j_%w";
    time_t t2;
    struct tm gtm, jtm, jtm2;
    char sg[64], sj[64];

    gmtime_r (&t, &gtm);
    julian_gmtime_r (&t, &jtm);
    jtm2= jtm;
    t2= julian_timegm (&jtm2);
    if (t2 != t || memcmp (&jtm, &jtm2, sizeof jtm) != 0) {
        printf ("*** Bug found t=%lld t2=%lld\n",
            (long long)t, (long long)t2);
        exit (12);
    }

    strftime (sg, sizeof sg, tfmt, &gtm);
    strftime (sj, sizeof sj, tfmt, &jtm);
    printf ("%13lld  PGC %26s  PJC %26s\n", (long long)t, sg, sj);
}

static void Test2 (int y, int m, int d) {
    Test3 (y, m, d, 0, 0, 0);
}

static void Test3 (int y, int m, int d, int H, int M, int S) {
    struct tm jtm;
    time_t t;

    memset (&jtm, 0, sizeof jtm);
    jtm.tm_year= y-1900;
    jtm.tm_mon=  m-1;
    jtm.tm_mday= d;
    jtm.tm_hour= H;
    jtm.tm_min=  M;
    jtm.tm_sec=  S;
    t= julian_timegm (&jtm);
    Test1 (t);
}

static void Test4 (int jday, int jsec) {
    JulianDate jd= {jday, jsec};
    double d, d2;
    JulianDate jd2= {-1, -1};
    int64_t ts;
    struct tm jtm, gtm;
static const char tfmt[] = "%Y-%m-%d.%H:%M:%S";
    char sj [32];
    char sg [32];

    printf ("\njulian date test %d,%d\n", jday, jsec);

    julian_jd2d (&jd, &d);
    julian_d2jd (d, &jd2);
    printf ("jd %d,%d -> %16.6f -> %d,%d\n"
        , (int)jd.jd_day, (int)jd.jd_sec
        , d
        , (int)jd2.jd_day, (int)jd2.jd_sec);

    julian_jd2tstamp (&jd, &ts);
    julian_tstamp2jd (ts, &jd2);
    printf ("jd %d,%d -> unix=%lld -> %d,%d\n"
        , (int)jd.jd_day, (int)jd.jd_sec
        , (long long)ts
        , (int)jd2.jd_day, (int)jd2.jd_sec);

    julian_d2tstamp (d, &ts);
    julian_tstamp2d (ts, &d2);
    printf ("jd %16.6f -> unix=%lld -> %16.6f\n"
        , d
        , (long long)ts
        , d2);

    {
       time_t t= ts;

       julian_gmtime_r (&t, &jtm);
       gmtime_r (&t, &gtm);
    }
    strftime (sg, sizeof sg, tfmt, &gtm);
    strftime (sj, sizeof sj, tfmt, &jtm);
    printf ("PGC: %s  PJC: %s\n", sg, sj);
}
