
/*
 * Operating System specific function (UNIX)
 *
 * $Header: unix.c 1.6 94/12/13 $
 * $Log:	unix.c,v $
 * Revision 1.6  94/12/13  11:36:31  arb
 * Changed use of timelocal() to SunOS 4 only... hope that's right!
 *
 * Revision 1.5  92/12/07  17:18:49  duplain
 * reformatted source.
 * 
 * Revision 1.4  92/10/19  09:35:10  duplain
 * Changed use of timelocal() to SunOS only... hope that's right.
 * 
 * Revision 1.3  92/10/06  12:12:51  duplain
 * Date/time conversion now done by mktime() or timelocal().
 * 
 * Revision 1.2  92/10/01  11:23:10  duplain
 * Added filesize().
 * 
 * Revision 1.1  92/09/29  18:02:28  duplain
 * Initial revision
 * 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifndef BSD42
#include <string.h>
#endif
#if !defined(BSD42) && !defined(SYSV2)
#include <utime.h>
#endif							/* not 4.2BSD and not SysVr2 */
#include <stdio.h>
#include "spark.h"
#include "date.h"

/*
 * return the length of a file
 */
Word
filesize(char *pathname)
{
	struct stat statb;

	if (stat(pathname, &statb) < 0)
		return (0);
	else
		return ((Word) statb.st_size);
}

/*
 * test for the existance of a file or directory
 */
Ftype
exist(char *pathname)
{
	struct stat statb;

	if (stat(pathname, &statb) < 0)
		return (NOEXIST);		/* assumes error was because file
								   doesn't exist... could be wrong! */
	if (statb.st_mode & S_IFDIR)
		return (ISDIR);
	else
		return (ISFILE);		/* might not be a regular file... */
}

/*
 * make a directory
 */
int
makedir(char *pathname)
{
	return (mkdir(pathname, 0777));
}

/*
 * stamp a file with date and time
 */
int
filestamp(Header *header, char *filename)
{
#if defined(BSD42) || defined(SYSV2)
	return (0);					/* not by supported 4.2BSD or SysV2 */
#else							/* not 4.2BSD or SysV */
	Date *date;
	struct tm tm;
	struct utimbuf utimbuf;
	time_t filetime;

	if ((header->load & (Word) 0xfff00000) != (Word) 0xfff00000)
		return (0);				/* not a timestamp */

#ifdef BSD
	bzero((char *) &tm, sizeof(tm));
#else							/* not BSD */
	memset((char *) &tm, '\0', sizeof(tm));
#endif							/* BSD */

	if (!(date = makedate(header)))
		return (-1);

	tm.tm_sec = date->second;
	tm.tm_min = date->minute;
	tm.tm_hour = date->hour;
	tm.tm_mday = date->day;
	tm.tm_mon = date->month - 1;
	tm.tm_year = date->year;
#if defined(sun) && defined(BSD)	/* SunOS 4 */
	filetime = timelocal(&tm);
#else							/* not SunOS */
	filetime = mktime(&tm);
#endif							/* SunOS */

	utimbuf.actime = filetime;
	utimbuf.modtime = filetime;
	return (utime(filename, &utimbuf));
#endif							/* not 4.2BSD and not SysV2 */
}
