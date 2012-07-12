#ifndef __STAT_H__
#define __STAT_H__

#include <sys/types.h>
#include <sys/time.h>

#define	_S_IFMT		0170000	/* These bits determine file type.  */
#define	_S_IFDIR	0040000	/* Directory.  */
#define	_S_IFCHR	0020000	/* Character device.  */
#define	_S_IFBLK	0060000	/* Block device.  */
#define	_S_IFREG	0100000	/* Regular file.  */
#define	_S_IFIFO	0010000	/* FIFO.  */
#define	_S_IFLNK	0120000	/* Symbolic link.  */
#define	_S_IFSOCK	0140000	/* Socket.  */

#define	S_ISUID	04000	/* Set user ID on execution.  */
#define	S_ISGID	02000	/* Set group ID on execution.  */
#define	S_ISVTX	01000	/* Save swapped text after use (sticky).  */
#define	S_IREAD	0400	/* Read by owner.  */
#define	S_IWRITE	0200	/* Write by owner.  */
#define	S_IEXEC	0100	/* Execute by owner.  */

#define	S_IRUSR	S_IREAD	/* Read by owner.  */
#define	S_IWUSR	S_IWRITE	/* Write by owner.  */
#define	S_IXUSR	S_IEXEC	/* Execute by owner.  */

#define	S_IRGRP	(S_IRUSR >> 3)	/* Read by group.  */
#define	S_IWGRP	(S_IWUSR >> 3)	/* Write by group.  */
#define	S_IXGRP	(S_IXUSR >> 3)	/* Execute by group.  */
/* Read, write, and execute by group.  */
#define	S_IRWXG	(S_IRWXU >> 3)

#define	S_IROTH	(S_IRGRP >> 3)	/* Read by others.  */
#define	S_IWOTH	(S_IWGRP >> 3)	/* Write by others.  */
#define	S_IXOTH	(S_IXGRP >> 3)	/* Execute by others.  */
/* Read, write, and execute by others.  */
#define	S_IRWXO	(S_IRWXG >> 3)

/* Read, write, and execute by owner.  */
#define	S_IRWXU	(__S_IREAD|__S_IWRITE|__S_IEXEC)

#define	__S_ISTYPE(mode, mask)	(((mode) & _S_IFMT) == (mask))

#define	S_ISDIR(mode)	 __S_ISTYPE((mode), _S_IFDIR)
#define	S_ISCHR(mode)	 __S_ISTYPE((mode), _S_IFCHR)
#define	S_ISBLK(mode)	 __S_ISTYPE((mode), _S_IFBLK)
#define	S_ISREG(mode)	 __S_ISTYPE((mode), _S_IFREG)

struct stat {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        _off_t     st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
		unsigned long int st_ctime_usec;

		int st_blksize;	/* Optimal block size for I/O.  */
#define	_STATBUF_ST_BLKSIZE	/* Tell code we have this member.  */

		int st_blocks;	/* Number of 512-byte blocks allocated.  */

		long int st_spare[2];

        };

DOSHAL int stat(const char * path, struct stat * buf);
DOSHAL int fstat(int fildes, struct stat *buf);
_inline int chmod(const char *path, mode_t mode) {return 0;}
_inline int fchmod(int fildes, mode_t mode) {return 0;}
_inline mode_t umask(mode_t cmask) {return cmask;}
DOSHAL int mkdir(const char *path, mode_t mode);
DOSHAL int lstat(const char *file_name, struct stat *buf);

#endif