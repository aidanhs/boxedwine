#ifndef __DIRENT_H__
#define __DIRENT_H__

typedef struct
{
	void* p;
} DIR;

struct dirent {
    char d_name[14];
};

DOSHAL DIR *opendir(const char *dirname);
DOSHAL struct dirent *readdir(DIR *dirp);
DOSHAL int closedir(DIR *dirp);

#endif