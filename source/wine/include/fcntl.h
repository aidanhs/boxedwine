#ifndef __COMPAT_FCNTL_H__
#define __COMPAT_FCNTL_H__

#include <sys/types.h>

//#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100	/* not fcntl */
#define O_EXCL		   0200	/* not fcntl */
//#define O_NOCTTY	   0400	/* not fcntl */
#define O_TRUNC		  01000	/* not fcntl */
#define O_APPEND	  02000
//#define O_NONBLOCK	  04000
//#define O_NDELAY	O_NONBLOCK
//#define O_SYNC		 010000
//#define FASYNC		 020000	/* fcntl, for BSD compatibility */
//#define O_DIRECT	 040000	/* direct disk access hint */
//#define O_LARGEFILE	0100000
//#define O_DIRECTORY	0200000	/* must be a directory */
//#define O_NOFOLLOW	0400000 /* don't follow links */

#define F_DUPFD            0    /* duplicate file descriptor */
#define F_GETFD            1    /* get file descriptor flags */
#define F_SETFD            2    /* set file descriptor flags */
#define F_GETFL            3    /* get file status flags */
#define F_SETFL            4    /* set file status flags */
#define F_GETLK            5    /* get record locking information */
#define F_SETLK            6    /* set record locking information */
#define F_SETLKW           7    /* set record locking info; wait if blocked */
#define F_FREESP           8    /* free a section of a regular file */

#define FD_CLOEXEC         1    /* close on exec flag for third arg of fcntl */

struct flock
  {
    short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
    short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
    off_t l_start;	/* Offset where the lock begins.  */
    off_t l_len;	/* Size of the locked area; zero means until EOF.  */
    pid_t l_pid;	/* Process holding the lock.  */
  };

#define	F_RDLCK	1	/* Read lock.  */
#define	F_WRLCK	2	/* Write lock.  */
#define	F_UNLCK	3	/* Remove lock.  */

DOSHAL int open(const char *path, int oflag, ... );

#endif