# timegm

timegm(3) implementation for platforms that don't have it (AIX is an example)

Type 'time_t' of the platform can be 32 or 64-bit long,
but type 'long long' has to be 64-bit long (or more).

Invalid dates (eg 2021.02.29) return -1, but this can be changed in the
future (so don't use this as a validator function).

Supported range for 64-bit time_t: 0-9999 years (might be increased later)
mind you, it is proleptic Gregorian (https://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar)
