/* $Id: datalimit.c,v 1.5 2007/08/24 18:17:31 fabio Exp fabio $ */

#if !defined(HAVE_SYS_RESOURCE_H)
#	ifdef WIN32
#		define HAVE_SYS_RESOURCE_H 0
#	else
#		define HAVE_SYS_RESOURCE_H 1
#	endif
#endif
#if !defined(HAVE_SYS_TIME_H)
#	ifdef WIN32
#		define HAVE_SYS_TIME_H 0
#	else
#		define HAVE_SYS_TIME_H 1
#	endif
#endif
#if !defined(HAVE_GETRLIMIT)
#	ifdef WIN32
#		define HAVE_GETRLIMIT 0
#	else
#		define HAVE_GETRLIMIT 1
#	endif
#endif

#if HAVE_SYS_RESOURCE_H == 1
#	if (HAVE_SYS_TIME_H == 1)
#		include <sys/time.h>
#	endif
#	include <sys/resource.h>
#endif

#ifndef RLIMIT_DATA_DEFAULT
#define RLIMIT_DATA_DEFAULT 1073741824	/* assume 1024MB by default */
#endif

#ifndef EXTERN
#   ifdef __cplusplus
#	define EXTERN extern "C"
#   else
#	define EXTERN extern
#   endif
#endif

EXTERN unsigned long getSoftDataLimit(void);

unsigned long
getSoftDataLimit(void)
{
#if HAVE_SYS_RESOURCE_H == 1 && HAVE_GETRLIMIT == 1 && defined(RLIMIT_DATA)
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_DATA, &rl);
    if (result != 0 || rl.rlim_cur == RLIM_INFINITY)
	return((unsigned long) RLIMIT_DATA_DEFAULT);
    else
	return((unsigned long) rl.rlim_cur);
#else
    return((unsigned long) RLIMIT_DATA_DEFAULT);
#endif

} /* end of getSoftDataLimit */
