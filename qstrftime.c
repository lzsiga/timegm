/* qstrftime.c */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* usage: %q => 1/2/3/4 (jan-mar, apr-jun, jul-aug, sep-dec)
   at compilation: -Dstrtime=qsrtftime
   compile with -std=c89 or newer

   when using with cronolog, add option --period, eg:
   cronolog --period=1months mylog.%YQ%q
 */

size_t qstrftime(char *buf, size_t bufsize, const char *fmt, const struct tm *tm);

#undef strftime
size_t qstrftime(char *buf, size_t bufsize, const char *fmt, const struct tm *tm) {
    static const char PERCENTQ[]= "%q";
    const char *pq;
    int quarter;
    const char *from;
    char *fmt2, *to;
    size_t retval;

    pq= strstr(fmt, PERCENTQ);
    if (!pq) {
        return strftime(buf, bufsize, fmt, tm);
    }
    quarter= (tm->tm_mon/3); /* 0..11 => 0..3 */

    fmt2= malloc(1+strlen(fmt));
    if (!fmt2) {
        errno= ENOMEM; /* not in POSIX */
        return 0;
    }

    from= fmt;
    to= fmt2;
    do {
        size_t slen= pq-from;
        if (slen) {
            memcpy(to, from, slen);
            to += slen;
            from += slen+2;
        }
        *to++ = '1'+quarter; /* '1'..'4' */
        pq= strstr(from, PERCENTQ);
    } while (pq);
    *to= '\0';

    retval= strftime(buf, bufsize, fmt2, tm);
    free(fmt2);
    return retval;
}

#ifdef TESTMYSELF
#include <stdio.h>

static void Test1(const char *fmt, const struct tm *tm) {
    size_t len;
    char buf[64];

    printf("%s -> ", fmt);
    fflush(stdout);
    len= qstrftime(buf, sizeof buf, fmt, tm);
    printf("len=%d: \"%s\"\n", (int)len, buf);
}

int main(int argc, const char **argv) {
    time_t t;
    struct tm tm;

    time(&t);
    tm= *localtime(&t);

    if (argc<2) {
        Test1("%Y%m%d.%H%M%S", &tm);
        Test1("%Y%m%d.%q.%H%M%S.%q", &tm);
    } else {
        int i;

        for (i=1; i<argc; ++i) {
            Test1(argv[i], &tm);
        }
    }
    return 0;
}
#endif
