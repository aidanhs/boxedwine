#define DIR void

DIR *hal_opendir(const char *name) 
{
	return 0;
}

struct dirent *hal_readdir(DIR *dir)
{
	return 0;
}

int hal_closedir(DIR *dir)
{
    return 0;
}