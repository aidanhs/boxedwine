#define __STDC__ 1
#include <sys/stat.h>
#include <windows.h>
#include <direct.h>

typedef unsigned int mode_t;
typedef unsigned int _dev_t;
typedef unsigned short _ino_t;
typedef unsigned short ino_t;
//typedef unsigned int _off_t;

#define	_S_IFLNK	0120000	/* Symbolic link.  */

int* getErrno();
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

ino_t fileino (const char *path)
{
	HANDLE hFile;
	BY_HANDLE_FILE_INFORMATION FileInformation;
	char fullpath[2*MAX_PATH+2];
	TCHAR *p;
	ino_t res;
//	LPVOID lpMsgBuf;

	if (!GetFullPathName (path, MAX_PATH, fullpath, &p)
			== strlen(fullpath))
		return 0;
	hFile = CreateFile (fullpath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;
	if(GetFileInformationByHandle (hFile, &FileInformation))
		res = ((ino_t) FileInformation.nFileIndexLow)
				^ ((ino_t) FileInformation.nFileIndexLow >> 16)
			^ ((ino_t) FileInformation.nFileIndexHigh)
				^ ((ino_t) FileInformation.nFileIndexHigh >> 16);
	else
		return 0;
//	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//	    NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//	    (LPTSTR) &lpMsgBuf, 0, NULL);
//	fprintf(stderr, "Error: %s\n", lpMsgBuf);
//	printf("%20s: %#10x %#10x %#10x \n", path, res, FileInformation.nFileIndexLow, FileInformation.nFileIndexHigh);
//	CloseHandle(hFile);
	return res;
}

int hal_lstat(const char *file_name, struct stat *buf)
{
	return 0;
}

int hal_fstatfs(int fd, struct statfs *buf)
{
	return 0;
}

int hal_stat(const char * path, struct stat * buf)
{
	char *ext;
	struct _stati64 _buf;
	
	if (_stati64(path, &_buf)==-1)
	{
		return -1;
	}

	buf->st_ino = _buf.st_ino;	
	buf->st_mode = _buf.st_mode;	
	buf->st_uid = _buf.st_uid;	
	buf->st_gid = _buf.st_gid;	
	buf->st_dev = _buf.st_dev;	
	buf->st_nlink = _buf.st_nlink;	
	buf->st_rdev = _buf.st_rdev;	
	buf->st_size = _buf.st_size;	
	buf->st_mode = _buf.st_mode;	
	buf->st_atime = _buf.st_atime;	
	buf->st_mtime = _buf.st_mtime;	
	buf->st_ctime = _buf.st_ctime;	
	buf->st_ino = fileino(path);
	ext = strrchr(path, '.');
	if (ext != NULL && !strcmp(ext, ".lnk")) {
		buf->st_mode &= ~_S_IFREG;
		buf->st_mode |= _S_IFLNK;
	}
	return 0;
}

int hal_fstat(int fildes, struct stat *buf)
{
	return _fstat32(fildes, (struct _stat32*)buf);
}

int hal_mkdir(const char *path, mode_t mode)
{
	return _mkdir(path);
}