/* 
 * File:   System.h
 * Author: hans
 *
 * Created on 18 January 2015, 13:47
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#ifdef OS_UNIX_LIKE
#define OS UNIX_LIKE
#endif

#ifdef OS_WINDOWS
#define OS WINDOWS
#endif

#ifdef OS

#if OS == UNIX_LIKE
#define OS_UNIX_LIKE
#undef OS_WINDOWS
#elif OS == WINDOWS
#define OS_WINDOWS
#undef OS_UNIX_LIKE
#else
#define OS_UNIX_LIKE
#undef OS_WINDOWS
#endif

#else

#define OS UNIX_LIKE
#define OS_UNIX_LIKE
#undef OS_WINDOWS
#endif

#endif	/* SYSTEM_H */

