#ifndef __DIRENT_H__
#define __DIRENT_H__

typedef struct
{
	void* p;
} DIR;

struct dirent {
    char d_name[14];
    int d_namlen;
    int d_type;
    int d_ino;
    int d_reclen;
};

#define DT_CHR 2
#define DT_DIR 4
#define DT_REG 8

DIR *winecrt_opendir(const char *dirname);
#define opendir winecrt_opendir

struct dirent *winecrt_readdir(DIR *dirp);
#define readdir winecrt_readdir

int winecrt_closedir(DIR *dirp);
#define closedir winecrt_closedir

#endif