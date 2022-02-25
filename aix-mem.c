/* aix-mem.c */

#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* these line arre here to prevent a 'missing prototype' warning */
void *mempcpy (void *dest, const void *src, size_t n);
void *memrchr (const void *s, int c, size_t n);
size_t strlcpy (char *dst, const char *src, size_t size);
size_t strlcat (char *dst, const char *src, size_t size);
char *stpncpy (char *dest, const char *src, size_t n);
void strmode (mode_t mode, char *bp);

void *mempcpy (void *dest, const void *src, size_t n)
{
    if (n==0) {
        return dest;
    } else {
        memcpy (dest, src, n);
        return (char *)dest + n;
    }
}

void *memrchr (const void *s, int c, size_t n)
{
    const char *p;

    if (!n) return NULL;
    p= (char *)s + n;

    while (--p >= (char *)s) {
        if (*p == (char)c) return (void *)p;
    }
    return NULL;
}

size_t strlcpy (char *dst, const char *src, size_t size)
{
    size_t srclen;
    size_t cpylen;

    srclen= strlen (src);

    if (size==0) return srclen;

    cpylen= size-1;
    if (cpylen>srclen) cpylen= srclen;

    if (cpylen) memcpy (dst, src, cpylen);
    dst[cpylen]= '\0';

    return srclen;
}

size_t strlcat (char *dst, const char *src, size_t size)
{
    size_t srclen;
    size_t dstlen;
    size_t cpylen;

    srclen= strlen (src);

    if (size==0) return srclen;

    dstlen= strnlen (dst, size);
    if (dstlen >= size-1) return dstlen + srclen;

    cpylen= size-1-dstlen;
    if (cpylen>srclen) cpylen= srclen;

    if (cpylen) memcpy (dst+dstlen, src, cpylen);
    dst[dstlen+cpylen]= '\0';

    return dstlen + srclen;
}

char *stpncpy (char *dest, const char *src, size_t n)
{
    size_t copylen= strnlen (src, n);
    size_t zerolen= n - copylen;
    if (copylen) memcpy (dest, src, copylen);
    if (zerolen) memset (dest+copylen, 0, zerolen);
    return dest+copylen;
}

static void strmode_rut (char *bp, unsigned rwx, unsigned extra, const char extrachars[2])
{
    if (rwx&04) bp[0]= 'r'; else bp[0]= '-';
    if (rwx&02) bp[1]= 'w'; else bp[1]= '-';
    if (extra) {
        if (rwx&01) bp[2]= extrachars[1]; else bp[2]=  extrachars[0]; /* s/S vagy t/T */
    } else {
        if (rwx&01) bp[2]= 'x'; else bp[2]=  '-';
    }
}

void strmode (mode_t mode, char *bp)
{
    switch (mode&S_IFMT) {
    case S_IFSOCK: *bp++= 's'; break;
    case S_IFLNK:  *bp++= 'l'; break;
    case S_IFREG:  *bp++= '-'; break;
    case S_IFBLK:  *bp++= 'b'; break;
    case S_IFDIR:  *bp++= 'd'; break;
    case S_IFCHR:  *bp++= 'c'; break;
    case S_IFIFO:  *bp++= 'p'; break;
    default:       *bp++= '?';
    }
    strmode_rut (bp, (mode&0700)>>6, mode&S_ISUID, "Ss"); bp += 3;
    strmode_rut (bp, (mode&0070)>>3, mode&S_ISGID, "Ss"); bp += 3;
    strmode_rut (bp, (mode&0007),    mode&S_ISVTX, "Tt"); bp += 3;
    *bp++= ' ';        /* ACL... */
    *bp++= '\0';
}
