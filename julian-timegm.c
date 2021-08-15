/* julian-timegm.c */

/* proleptic Julian calendar: https://en.wikipedia.org/wiki/Proleptic_Julian_calendar */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "julian-timegm.h"

time_t julian_timegm (struct tm *tm);
struct tm *julian_gmtime_r (const time_t *t, struct tm *tm);

static const int32_t epoch_year= 1970; /* unix timestamp epoch */
static const int32_t base_year=  -9999;  /* start of a 100-year period */
static const int32_t secPday=    24L*60*60;

static const int msum [2][12] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},       /* normal years */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}        /* leap years */
};
static const int mlen [2][12] = {
    { 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30, 31},
    { 31, 29, 31, 30,  31,  30,  31,  31,  30,  31,  30, 31}
};

/* Unix Epoch Time is PJD 1969-12-19 which 13 days before PJD 1970-01-01 */

time_t julian_timegm (struct tm *tm) {
    static const int tmstr_year= 1900; /* base of 'tm_year' in 'struct tm' */
    int year, ytmp, dtmp, ytmpe, dtmpe;
    int isleapyear;
    long long t;

    if (!tm) return -1;

    year = tm->tm_year + tmstr_year;
    isleapyear= (year%4==0) - (year%100==0);

    if (year<-9999    || year>9999  ||
        tm->tm_mon<0  || tm->tm_mon>11 ||
        tm->tm_mday<1 || tm->tm_mday>mlen[isleapyear][tm->tm_mon] ||
        tm->tm_hour<0 || tm->tm_hour>23 ||
        tm->tm_min<0  || tm->tm_min>59 ||
        tm->tm_min<0  || tm->tm_sec>59) return -1;

/* days between 'current year' and 'epoch_year' has to be calculated
   in three steps: */

/* 1. days between current year and 'base_year' */
    ytmp = year - base_year;
    dtmp = ytmp*365 + ytmp/4;

/* 2. days between 'epoch year' and 'base_year' */
    ytmpe = epoch_year - base_year;
    dtmpe = ytmpe*365 + ytmpe/4;

/* 3. days between 'current year' and 'epoch_year' */
    t  = dtmp - dtmpe;

/* 4. 13 days correction */
    t += 13;

    t += msum[isleapyear][tm->tm_mon];
    t += tm->tm_mday-1;

    tm->tm_wday = (t+4)%7;/* 0..6=Sun..Sat; adding 4 to adjust 1970.01.01.=Thursday; */
    if (tm->tm_wday<0) tm->tm_wday += 7;
    tm->tm_yday = msum[isleapyear][tm->tm_mon] + tm->tm_mday-1;
    tm->tm_isdst= 0;

    t  = t*24 + tm->tm_hour;
    t  = t*60 + tm->tm_min;
    t  = t*60 + tm->tm_sec;

#if LONG_MAX == 2147483647L
    if (t<LONG_MIN || t>LONG_MAX) {
        t= -1;
    }
#endif

    return t;
}

struct tm *julian_gmtime_r (const time_t *t, struct tm *tm) {
/* rebase from: GD 1970-01-01 = JD 1969-12-19 */
/*          to: JD-9999-01-01 */
static const int32_t dayoffset= 4371664;
static const int32_t yearoffs=  9999;
    int64_t t64= *t;
    int32_t y4, year, tmp;
    int32_t hms, days, yday, mon, mday;
    int isleapyear;

    hms = t64%secPday;
    if (hms<0) hms += secPday;
    days = (t64-hms)/secPday + dayoffset;

    memset (tm, 0, sizeof *tm);
    tm->tm_sec= hms%60;
    tm->tm_min= hms%(60*60)/60;
    tm->tm_hour=hms/(60*60);

    tmp= days;
    y4=  tmp/(3*365+366);
    tmp= tmp%(3*365+366);
    year= 4*y4 + tmp/365;
    isleapyear= (year+1)%4==0;
    yday= tmp%365;

    mon= yday/31;
    if (mon>11) mon= 11;
    mday= yday-msum[isleapyear][mon];
    while (mday<0 && mon>0) {
        --mon;
        mday= yday-msum[isleapyear][mon];
    }
    while (mday>=mlen[isleapyear][mon] && mon<11) {
        ++mon;
        mday= yday-msum[isleapyear][mon];
    }

    tm->tm_year= year-yearoffs-1900;
    tm->tm_mon=  mon;
    tm->tm_mday= 1+mday;

    tm->tm_yday= yday;
    tm->tm_wday= (days+1)%7;

    return tm;
}

int julian_jd2d (const JulianDate *jd, double *d) {
    *d= (double)jd->jd_day + jd->jd_sec/86400.0;
    return 0;
}

int julian_d2jd (double d, JulianDate *jd) {
    int32_t dtmp;
    if (d<=INT32_MIN || d>=INT32_MAX) {
        jd->jd_day= 0;
        jd->jd_sec= 0;
        return -1;
    }
    dtmp= d + 0.5/86400;
    jd->jd_day= dtmp;
    if (dtmp>=d) jd->jd_sec= 0;
    else         jd->jd_sec= (d-dtmp)*86400;
    return 0;
}

int julian_tstamp2jd (int64_t unixts, JulianDate *jd) {
    int64_t tmp= unixts - JDATE_START_UNIXTS;
    int64_t tday= tmp/86400;
    int32_t tsec;

    if (tday<=INT32_MIN || tday>=INT32_MAX) {
        jd->jd_day= 0;
        jd->jd_sec= 0;
        return -1;
    }
    jd->jd_day= (int32_t)tday;
    tsec= tmp%86400;
    if (tsec<0) tsec += 86400;
    jd->jd_sec= tsec;
    return 0;
}

int julian_jd2tstamp (const JulianDate *jd, int64_t *unixts) {
    *unixts = jd->jd_day*86400LL + jd->jd_sec + JDATE_START_UNIXTS;
    return 0;
}

int julian_tstamp2d (int64_t unixts, double *d) {
    *d= (unixts - JDATE_START_UNIXTS)/86400.0;
    return 0;
}

int julian_d2tstamp (double d, int64_t *unixts) {
    *unixts = d*86400 + JDATE_START_UNIXTS;
    return 0;
}
