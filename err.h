#ifndef _ERR_
#define _ERR_

#define SYSERR(msg) syserr("%s, line %d: %s:", __FILE__, __LINE__, msg);

/* wypisuje informacje o blednym zakonczeniu funkcji systemowej
i konczy dzialanie */
extern void syserr(const char *fmt, ...);

/* wypisuje informacje o bledzie i konczy dzialanie */
extern void fatal(const char *fmt, ...);

#endif
